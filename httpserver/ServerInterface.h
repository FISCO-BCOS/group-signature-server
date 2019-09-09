/*
	This file is part of FISCO BCOS.

	FISCO BCOS is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FISCO BCOS is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FISCO BCOS.  If not, see <http://www.gnu.org/licenses/>.
*/

/* @file: ServerInterface.h
 * @author: fisco-dev
 * @date: 2018.03.07
 */
#pragma once
#include <tuple>
#include <vector>
#include <memory>
#include <string>
#include "jsonrpccpp/common/procedure.h"
#include "jsonrpccpp/server/iprocedureinvokationhandler.h"
#include "jsonrpccpp/server/abstractserverconnector.h"
#include "jsonrpccpp/server/requesthandlerfactory.h"

#include "easylog/easylog.h"

template <class I>
using AbstractMethodPointer = void (I::*)(Json::Value const &_parameter, Json::Value &_result);

template <class I>
using AbstractNotificationPointer = void (I::*)(Json::Value const &_parameter);

struct RpcModule
{
    std::string name;
    std::string version;
};

//abstract interface of method
template <typename T>
class ServerInterface
{
public:
    using MethodPointer = AbstractMethodPointer<T>;
    using NotificationPointer = AbstractNotificationPointer<T>;
    using MethodBinding = std::tuple<jsonrpc::Procedure, AbstractMethodPointer<T>>;
    using NotificationBinding = std::tuple<jsonrpc::Procedure, AbstractNotificationPointer<T>>;
    using MethodVec = std::vector<MethodBinding>;
    using NotificationVec = std::vector<NotificationBinding>;
    using RpcModuleVec = std::vector<RpcModule>;

    virtual ~ServerInterface() {}
    MethodVec const &methods() const { return m_methods; }
    NotificationVec const &notifications() const { return m_notifications; }

    //rpc version, for version manager
    virtual RpcModuleVec ImplementedModules() const = 0;

protected:
    void bind_and_add_method(jsonrpc::Procedure const &proc,
                             MethodPointer method_pointer)
    {
        m_methods.emplace_back(proc, method_pointer);
    }

    void bind_and_add_notification(jsonrpc::Procedure const &proc,
                                   NotificationPointer pointer)
    {
        m_notifications.emplace_back(proc, pointer);
    }

private:
    MethodVec m_methods;
    NotificationVec m_notifications;
};

template <class... Is>
class ModularServer : public jsonrpc::IProcedureInvokationHandler
{
public:
    ModularServer() : m_handler(jsonrpc::RequestHandlerFactory::createProtocolHandler(jsonrpc::JSONRPC_SERVER_V2, *this))
    {
        m_handler->AddProcedure(jsonrpc::Procedure("rpc_modules", jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, NULL));
        m_implemented_modules = Json::objectValue;
    }

    //rpc_module default callback function
    inline virtual void modules(const Json::Value &request, Json::Value &response)
    {
        response = m_implemented_modules;
    }

    /*
     *@function: start listen service for m_connector to receive requests
     *@ret: true: listen service is started successfully
     *      false: listen service is failed to start
     */
    virtual bool StartListening()
    {
        LOG(DEBUG) << "start listening service for all connectors";
        for (auto const &connector : m_connectors)
        {
            if (false == connector->StartListening())
                return false;
        }
        return true;
    }

    //@function: stop listen service for all connectors
    virtual void StopListening()
    {
        LOG(DEBUG) << "stop listening service for all connectors";
        for (auto const &connector : m_connectors)
            connector->StopListening();
    }

    unsigned add_connector(jsonrpc::AbstractServerConnector *connector)
    {
        m_connectors.emplace_back(connector);
        connector->SetHandler(m_handler.get());
        return m_connectors.size();
    }

    jsonrpc::AbstractServerConnector *get_connector(unsigned int index)
    {
        return m_connectors[index].get();
    }

    //virtual function of IProcedureInvokationHandler for rpc method
    virtual void HandleMethodCall(jsonrpc::Procedure &proc,
                                  Json::Value const &param, Json::Value &output)
    {
        if (proc.GetProcedureName() == "rpc_modules")
            modules(param, output);
    }

    virtual void HandleNotificationCall(jsonrpc::Procedure &proc,
                                        Json::Value const &param)
    {
        (void)proc;
        (void)param;
    }

    virtual ~ModularServer()
    {
        StopListening();
    }

protected:
    //connections
    std::vector<std::unique_ptr<jsonrpc::AbstractServerConnector>> m_connectors;
    //callbacks
    std::unique_ptr<jsonrpc::IProtocolHandler> m_handler;
    //using json implementation
    Json::Value m_implemented_modules;
};

//abstract interface of server(http server, safe http server, etc.)
//these interface implement: startListening, stopLisening interfaces,etc.
template <class T, class... Is>
class ModularServer<T, Is...> : public ModularServer<Is...>
{
public:
    using MethodPointer = AbstractMethodPointer<T>;
    using NotificationPointer = AbstractNotificationPointer<T>;
    ModularServer<T, Is...>(T *_interface, Is *... _is) : ModularServer<Is...>(_is...),
                                                          m_interface(_interface)
    {
        register_procedures();
    }

    //register procedures
    inline void register_procedures()
    {
        for (auto const &method : m_interface->methods())
        {
            //method map:
            //  key: method name;
            //  value: method pointer
            //LOG(DEBUG)<<"register method:"<<std::get<0>(method).GetProcedureName();
            m_methods[std::get<0>(method).GetProcedureName()] =
                std::get<1>(method);
            //register RPC procedure
            this->m_handler->AddProcedure(std::get<0>(method));
        }
        //method interface must implement "notification()" method
        for (auto const &notification : m_interface->notifications())
        {
            // LOG(DEBUG)<<"register notification:"
            //           <<std::get<0>(notification).GetProcedureName();
            m_notifications[std::get<0>(notification).GetProcedureName()] = std::get<1>(notification);
            this->m_handler->AddProcedure(std::get<0>(notification));
        }
        //rpc versions
        for (auto const &module : m_interface->ImplementedModules())
            this->m_implemented_modules[module.name] = module.version;
    }

    //virtual function of IProcedureInvokationHandler for rpc method
    virtual void HandleMethodCall(jsonrpc::Procedure &proc,
                                  Json::Value const &param, Json::Value &output)
    {
        //get method pointer
        auto pointer = m_methods.find(proc.GetProcedureName());
        if (pointer != m_methods.end())
        {
            //callback method
            (m_interface.get()->*(pointer->second))(param, output);
            LOG(DEBUG) << "call back specified method end, output:" << output;
        }
        else
        {
            LOG(DEBUG) << (proc.GetProcedureName()) << " has no specified method,"
                       << "use default method";
            ModularServer<Is...>::HandleMethodCall(proc, param, output);
        }
    }

    //virtual function of IProcedureInvokationHandler for notification
    virtual void HandleNotificationCall(jsonrpc::Procedure &proc,
                                        Json::Value const &param)
    {
        //callback interface method
        auto notification_pointer = m_notifications.find(proc.GetProcedureName());
        if (notification_pointer != m_notifications.end())
        {
            (m_interface.get()->*(notification_pointer->second))(param);
            LOG(DEBUG) << "call back method " << (proc.GetProcedureName()) << " ended";
        }
        else
        {
            ModularServer<Is...>::HandleNotificationCall(proc, param);
        }
    }

protected:
    std::unique_ptr<T> m_interface;
    std::map<std::string, MethodPointer> m_methods;
    std::map<std::string, NotificationPointer> m_notifications;
};
