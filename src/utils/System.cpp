#include "System.h"

#include <QDir>

System::System() : lockFile_(QDir::temp().absoluteFilePath("qsnip.lock")) {
}

System::~System() {
    lockFile_.unlock();
}

bool System::isLaunched() {
    return !lockFile_.tryLock(100);
}
