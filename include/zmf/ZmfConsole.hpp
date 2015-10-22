#ifndef ZMF_ZMFCONSOLE_H
#define ZMF_ZMFCONSOLE_H

#include "IZmfInstanceController.hpp"
#include <memory>


namespace zmf {

    /**
     * @details Console utility to display a console for controlling an instance
     * @author Jonas Grunert
     * @date created on 7/7/15.
     */
    class ZmfConsole {

    public:
        ZmfConsole(std::shared_ptr<IZmfInstanceController> instanceController);

        /**
         * Starts running the ZMF console, blocking call
         */
        void startConsole();

    private:
        std::shared_ptr<IZmfInstanceController> instanceController_;

        void printHelpText();

        bool tryStringToModuleId(std::string str, zmf::data::ModuleUniqueId& outRef);
    };
}

#endif //ZMF_ZMFCONSOLE_H
