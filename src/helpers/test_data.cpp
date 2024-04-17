#include "helpers/test_data.h"

#include <stdexcept>


namespace Tests {

    std::string getTestDataDir() {
        char* testDataDir = std::getenv("TEST_DATA_DIR");
        if (!testDataDir) {
            throw std::runtime_error("TEST_DATA_DIR is not set");
        }
        return testDataDir;
    }

} // namespace Tests