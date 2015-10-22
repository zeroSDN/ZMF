#ifndef ZMF_ZMFINSTANCE_H
#define ZMF_ZMFINSTANCE_H

#ifdef __GNUC__
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif

#include "AbstractModule.hpp"
#include "IZmfInstanceController.hpp"

namespace zmf {
    namespace instance {
        /**
         * Launcher class to create and start new ZMF instances
         * @author Jonas Grunert
         * @author Jan Strauß
         * @author Andre Kutzleb
         * Created on 7/28/15.
        */
        class ZmfInstance {
        public:
            /**
             * Starting options when creating and starting a new ZMF instance
             */
            enum StartOption {
                /**
                 * NO auto enable, will NOT try to enable the module as soon as possible
                 */
                        NO_AUTO_ENABLE,
                /**
                 * Will NOT exit if module enabling fails
                 */
                        NO_EXIT_WHEN_ENABLE_FAILED,
                /**
                 *
                 * Peer discovery will NOT wait until all active modules discovered
                 */
                        NO_PEER_DISCOVERY_WAIT,
                /**
                 *
                 * This instance will not connect to modules with the same type id
                 */
                        NO_EQUAL_MODULE_INTERCONNECT
            };


            /**
             * Creates and starts  a new ZMF instance
             * @param instanceModule
             * 		the module to start
             * @param trackModuleStates deprecated - will be ignored
             * @param moduleAutoEnable If true tries to enable the module as soon as possible
             * @param exitWhenEnableFail If true will exit if module enabling fails
             * @param peerDiscoveryWait If true the peer discovery will wait until all active modules discovered
             * @param disableEqualModuleInterconnect If true peer discovery will ignore all messages from peer with the same type id
             * @param cfgPath Optional Path to the user defined config file
             * @return The controller to control the instance or shared_ptr(nullptr) if fails
             * @deprecated use createInstance with StartingOptions
            */
            DEPRECATED static std::shared_ptr<zmf::IZmfInstanceController> startInstance(
                    std::shared_ptr<zmf::AbstractModule> instanceModule,
                    bool trackModuleStates,
                    bool moduleAutoEnable,
                    bool exitWhenEnableFail,
                    bool peerDiscoveryWait,
                    bool disableEqualModuleInterconnect,
                    const std::string& cfgPath = "");


            /**
             * Creates and starts a new ZMF instance and uses default configuration file.
             * @param instanceModule
             * 		the module to start
             * @param startOptions Starting options for ZMF
             * @param cfgPath Optional Path to the user defined config file
             * @return The controller to control the instance or shared_ptr(nullptr) if fails
             */
            static std::shared_ptr<zmf::IZmfInstanceController> startInstance(
                    std::shared_ptr<zmf::AbstractModule> instanceModule,
                    std::vector<StartOption> startOptions,
                    const std::string& cfgPath = "");

        };

    }
}


#endif //ZMF_ZMFINSTANCE_H
