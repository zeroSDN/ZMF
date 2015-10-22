/**
 * @author Jan Strauß
 * @date created on 7/22/15.
*/


#include "ZmfLogging.hpp"

std::atomic_bool zmf::logging::ZmfLogging::initialized = ATOMIC_VAR_INIT(false);

void zmf::logging::ZmfLogging::initializeLogging(const std::string& moduleName, const std::string& path) {
    bool expected = false;
    if (!initialized.compare_exchange_strong(expected, true)) {
        return;
    }

    // Create Logger
    std::cout << "Starting ZMF Logger" << &std::endl;

    // Create and start logging
    Poco::AutoPtr<Poco::FileChannel> loggerFileChannel(new Poco::FileChannel());
    loggerFileChannel->setProperty("path", moduleName + ".log");
    Poco::AutoPtr<Poco::ConsoleChannel> loggerConsoleChannel(new Poco::ConsoleChannel());
    Poco::AutoPtr<Poco::SplitterChannel> loggerSplitChannel(new Poco::SplitterChannel());

    loggerSplitChannel->addChannel(loggerFileChannel);
    loggerSplitChannel->addChannel(loggerConsoleChannel);

    Poco::AutoPtr<Poco::PatternFormatter> loggerPatternFormatter(
            new Poco::PatternFormatter("%Y-%m-%d %H:%M:%S.%i %q | %s - %t"));
    Poco::AutoPtr<Poco::FormattingChannel> loggerFormatterChannel(
            new Poco::FormattingChannel(loggerPatternFormatter, loggerSplitChannel));

    Poco::Logger::root().setChannel(loggerFormatterChannel);
    Poco::Logger::root().setLevel(Poco::Message::PRIO_TRACE);
    Poco::Logger::root().information("logger started");

    if (path.empty()) {
        Poco::Logger::root().information("no config file provided, default values used");
        return;
    }
    try {
        Poco::Util::LoggingConfigurator lc;
        Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> logging_conf = Poco::AutoPtr<Poco::Util::PropertyFileConfiguration>(
                new Poco::Util::PropertyFileConfiguration());
        logging_conf->load(Poco::Path::expand(path));
        lc.configure(logging_conf);
        std::cout << "configured logger from " + path << &std::endl;
    } catch (const Poco::Exception& e) {
        std::cout << "failed to configure logger from " + path + ": " << e.displayText() << &std::endl;
    }
}

Poco::Logger& zmf::logging::ZmfLogging::getLogger(std::string name) {
    initializeLogging("zmf");
    return Poco::Logger::get(name);
}

zmf::logging::ZmfLoggingContainer* zmf::logging::ZmfLogging::getLoggerContainer(std::string name) {
    return new zmf::logging::ZmfLoggingContainer(getLogger(name));
}
