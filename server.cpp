#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <string>
#include <memory>
#include <map>
#include <signal.h>
#include <group_sig/devcore/CommonStruct.h>
#include "easylog/easylog.h"
#include "httpserver/TestInterface.h"
#include "httpserver/GroupSigRpc.h"
#include "httpserver/RingSigRpc.h"

#define MAX_PATH_LEN 255
using namespace jsonrpc;
INITIALIZE_EASYLOGGINGPP

void usage(char **argv)
{
    std::cout << "Usage:" << std::endl;
    std::cout << (argv[0]) << " ${listen_port} ${http_thread_num}" << std::endl;
}

static map<string, unsigned int> s_mlogIndex;
void rolloutHandler(const char *filename, std::size_t)
{
    std::stringstream stream;

    map<string, unsigned int>::iterator iter = s_mlogIndex.find(filename);
    if (iter != s_mlogIndex.end())
    {
        stream << filename << "." << iter->second++;
        s_mlogIndex[filename] = iter->second++;
    }
    else
    {
        stream << filename << "." << 0;
        s_mlogIndex[filename] = 0;
    }
    boost::filesystem::rename(filename, stream.str().c_str());
}

void initEasylogging(const std::string &log_conf)
{
    std::string logconf = log_conf;
    if ('/' != log_conf[0])
    {
        char abs_path[MAX_PATH_LEN];
        getcwd(abs_path, MAX_PATH_LEN);
        logconf = abs_path;
        logconf = logconf + "/" + log_conf;
    }

    el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport); // Enables support for multiple loggers
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    if (el::base::utils::File::pathExists(logconf.c_str(), true))
    {
        el::Logger *fileLogger = el::Loggers::getLogger("fileLogger"); // Register new logger
        el::Configurations conf(logconf.c_str());
        el::Configurations allConf;
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::Enabled));
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::ToFile));
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::ToStandardOutput));
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::Format));
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::Filename));
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::SubsecondPrecision));
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::MillisecondsWidth));
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::PerformanceTracking));
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::MaxLogFileSize));
        allConf.set(conf.get(el::Level::Global, el::ConfigurationType::LogFlushThreshold));
        allConf.set(conf.get(el::Level::Trace, el::ConfigurationType::Enabled));
        allConf.set(conf.get(el::Level::Debug, el::ConfigurationType::Enabled));
        allConf.set(conf.get(el::Level::Fatal, el::ConfigurationType::Enabled));
        allConf.set(conf.get(el::Level::Error, el::ConfigurationType::Enabled));
        allConf.set(conf.get(el::Level::Warning, el::ConfigurationType::Enabled));
        allConf.set(conf.get(el::Level::Verbose, el::ConfigurationType::Enabled));
        allConf.set(conf.get(el::Level::Info, el::ConfigurationType::Enabled));
        el::Loggers::reconfigureLogger("default", allConf);
        el::Loggers::reconfigureLogger(fileLogger, conf);
    }
    el::Helpers::installPreRollOutCallback(rolloutHandler);
}

//signal handler
void signal_handler(int sig)
{
    std::cout << "get sig:" << sig << " ,exit now" << std::endl;
    exit(0);
}

int main(int argc, char *argv[])
{
    boost::program_options::options_description server_options("group sig and ring sig RPC");
    server_options.add_options()("port,p", boost::program_options::value<int>(),
                                 "listen port of group sig and ring sig RPC")("http_thread_num,n", boost::program_options::value<int>(),
                                                                              "thread num of http server used to deal with requests")("log_path,l", boost::program_options::value<std::string>(),
                                                                                                                                      "path of log configuration")("ssl_cert,s", boost::program_options::value<std::string>(),
                                                                                                                                                                   "path of ssl certification")("ssl_key,k", boost::program_options::value<std::string>(),
                                                                                                                                                                                                "key to access ssl certification")("help,h", "help of group sig and ring sig rpc");

    boost::program_options::variables_map vm;
    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, server_options), vm);
    }
    catch (...)
    {
        std::cout << "invalid input" << std::endl;
        return 1;
    }
    //help information
    if (vm.count("help") || vm.count("h"))
    {
        std::cout << server_options << std::endl;
        return 0;
    }

    std::string log_path = "log.conf";
    if (vm.count("log_path") || vm.count("l"))
        log_path = vm["log_path"].as<std::string>();
    initEasylogging(log_path);

    int port = 8003;
    if (vm.count("port") || vm.count("p"))
        port = vm["port"].as<int>();

    int http_thread_num = 50;
    if (vm.count("http_thread_num") || vm.count("n"))
        http_thread_num = vm["http_thread_num"].as<int>();

    std::cout << "port:" << port << " "
              << "thread:" << http_thread_num << std::endl;
    LOG(INFO) << "BEGIN MAIN TEST HTTP SERVER";
    std::string ssl_cert = "";
    if (vm.count("ssl_cert") || vm.count("s"))
        ssl_cert = vm["ssl_cert"].as<std::string>();

    std::string ssl_key = "";
    if (vm.count("ssl_key") || vm.count("k"))
        ssl_key = vm["ssl_key"].as<std::string>();

    //create and start httpserver
    AbstractServerConnector *p_server = (new HttpServer(port, ssl_cert,
                                                        ssl_key, http_thread_num));
    GroupSigRpc *inter = (new GroupSigRpc());
    RingSigRpc *ring_sig_rpc = (new RingSigRpc());

    if (inter && ring_sig_rpc && p_server)
    {
        using ServiceType = ModularServer<GroupSigRpc, RingSigRpc>;
        std::shared_ptr<ServiceType> test_server(new ServiceType(std::move(inter), std::move(ring_sig_rpc)));

        LOG(DEBUG) << "ADD HTTP CONNECTOR TO test_server";
        std::cout << "ADD HTTP CONNECTOR TO test_server" << std::endl;
        test_server->add_connector(std::move(p_server));
        //test_server.add_connector(p_server);

        LOG(DEBUG) << "start listening on port " << port;
        std::cout << "start listening on port " << port << std::endl;
        if (false == test_server->StartListening())
        {
            LOG(ERROR) << "START LISTENING ON PORT 2000 FAILED";
            exit(0);
        }
        //deal with signals
        signal(SIGABRT, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);

        while (true)
        {
            sleep(1000);
        }
    }
}
