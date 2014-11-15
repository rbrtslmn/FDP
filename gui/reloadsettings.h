#ifndef RELOADSETTINGS_H
#define RELOADSETTINGS_H

namespace fdp {
namespace gui {

enum ReloadSettings {
    ReloadAfterRest = 1,
    ReloadFWError = 2,
    ReloadTimeout = 4,
    ReloadNetError = 8
};

} // end of namespace gui
} // end of namespace fdp

#endif // RELOADSETTINGS_H
