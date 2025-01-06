
#include <PersistantStorage/FlashStorage.hpp>

#define DOUBLE_WORD_ERASED (0xFFFFFFFFFFFFFFFFULL)

FlashStorage::FlashStorage() : PersistantStorage(FLASH_BASE, FLASH_SIZE) {}

FlashStorage::FlashStorage(uint32_t start_address, uint32_t len) : PersistantStorage(start_address, len) {}

void FlashStorage::init()
{
    /* Nothing to do */
}

void FlashStorage::deinit()
{
    /* Nothing to do */
}

void FlashStorage::erase()
{
    erase(start_address, len);
}

void FlashStorage::erase(uint32_t start_address, uint32_t len)
{
    uint32_t error = 0;
    FLASH_EraseInitTypeDef erase_init = {0};

    if (!is_start_of_page(start_address) || !is_start_of_page(start_address + len) || !is_within_range(start_address, len)) {
        return;
    }

    HAL_FLASH_Unlock();
    __disable_irq();

    for (uint8_t i = 0; i < 256; i++) {

        if (start_address + i * FLASH_PAGE_SIZE >= start_address + len) {
            break;
        }

        /* Set up bank and page to erase */
        erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
        erase_init.Banks = get_bank(start_address + i * FLASH_PAGE_SIZE);
        erase_init.Page = get_page(start_address + i * FLASH_PAGE_SIZE);
        erase_init.NbPages = 1;

        HAL_FLASHEx_Erase(&erase_init, &error);
    }

    __enable_irq();
    HAL_FLASH_Lock();
}

void FlashStorage::write8(uint32_t address, uint8_t value)
{
    /* Only double-words can be written to 8-byte regions that were erased.
     * The only meaningful implementation is to write the uint8_t as a uint64_t
     * and waste the remaining 7 bytes. */

    uint64_t double_word = value;
    write(address, (uint8_t*)&double_word, 8);
}

void FlashStorage::write32(uint32_t address, uint32_t value)
{
    /* Only double-words can be written to 8-byte regions that were erased.
     * The only meaningful implementation is to write the uint32_t as a uint64_t
     * and waste the remaining 4 bytes. */

    uint64_t double_word = value;
    write(address, (uint8_t*)&double_word, 8);
}

void FlashStorage::write(uint32_t address, const uint8_t* data, uint32_t len)
{
    if (!is_within_range(address, len) || !is_double_word_aligned(address) || !is_range_erased(address, len)) {
        return;
    }

    uint64_t double_word;
    uint32_t double_words = (len % 8 == 0) ? len / 8 : len / 8 + 1;

    HAL_FLASH_Unlock();
    __disable_irq();

    for (uint32_t i = 0; i < double_words; i++) {

        if (len % 8 == 0 || i < double_words - 1) {
            double_word = *((uint64_t*)(data + i * 8));
        } else {
            /* Special case: Last few bytes don't fill double word */
            double_word = DOUBLE_WORD_ERASED;
            for (uint32_t j = 0; j < len % 8; j++) {
                *((uint8_t*)(&double_word) + j) = data[i * 8 + j];
            }
        }

        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address + i * 8, double_word);
    }

    __enable_irq();
    HAL_FLASH_Lock();
}

void FlashStorage::read8(uint32_t address, uint8_t* value)
{
    *value = *((uint8_t*)(address));
}

void FlashStorage::read32(uint32_t address, uint32_t* value)
{
    *value = *((uint32_t*)(address));
}

void FlashStorage::read(uint32_t address, uint8_t* data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        data[i] = *((uint8_t*)(address + i));
    }
}

bool FlashStorage::is_start_of_page(uint32_t address)
{
    return (address % FLASH_PAGE_SIZE) == 0;
}

bool FlashStorage::is_within_range(uint32_t address, uint32_t len)
{
    bool within_range = true;

    within_range = within_range && (address - FLASH_BASE) <= FLASH_SIZE;
    within_range = within_range && (address + len - FLASH_BASE) <= FLASH_SIZE;

    return within_range;
}

bool FlashStorage::is_double_word_aligned(uint32_t address)
{
    return (address % 8) == 0;
}

bool FlashStorage::is_range_erased(uint32_t address, uint32_t len)
{
    bool range_erased = true;
    uint32_t double_words = (len % 8 == 0) ? len / 8 : len / 8 + 1;

    for (uint32_t i = 0; i < double_words; i++) {
        range_erased = range_erased && (*((uint64_t*)(address + i * 8)) == DOUBLE_WORD_ERASED);
    }

    return range_erased;
}

uint32_t FlashStorage::get_page(uint32_t address)
{
    uint32_t page = 0;

    if (address < (FLASH_BASE + FLASH_BANK_SIZE)) {
        page = (address - FLASH_BASE) / FLASH_PAGE_SIZE;
    } else {
        page = (address - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }

    return page;
}

uint32_t FlashStorage::get_bank(uint32_t address)
{
    if (address < (FLASH_BASE + FLASH_BANK_SIZE)) {
        return FLASH_BANK_1;
    } else {
        return FLASH_BANK_2;
    }
}