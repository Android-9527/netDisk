#ifndef __ECHOSERVER_H__

#include "ThreadPool.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "WordQuery.h"
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;

class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
    {

    }
    void process()
    {
        // 获取 WordQuery 单例对象
        WordQuery& wordQuery = WordQuery::getInstance("dictCn.txt", "indexCn.txt");
        
        // 去除消息中可能的空白字符
        string query = _msg;
        query.erase(0, query.find_first_not_of(" \t\r\n"));
        query.erase(query.find_last_not_of(" \t\r\n") + 1);
        
        if (query.empty()) {
            _con->sendInLoop("请输入查询关键词");
            return;
        }
        
        // 进行查询
        auto suggestions = wordQuery.query(query);
        
        // 构建响应消息
        std::ostringstream oss;
        oss << "查询: '" << query << "'\n";
        
        if (suggestions.empty()) {
            oss << "没有找到匹配的结果\n";
        } else {
            oss << "找到 " << suggestions.size() << " 个结果:\n";
            for (const auto& suggestion : suggestions) {
                oss << "  " << suggestion << "\n";
            }
        }
        
        // 将查询结果发送给客户端
        _con->sendInLoop(oss.str());
        // _con->sendInLoop(_msg);
    }
private:
    string _msg;
    TcpConnectionPtr _con;
};

class EchoServer
{
public:
    EchoServer(size_t threadNum, size_t queSize
               , const string &ip
               , unsigned short port)
    : _pool(threadNum, queSize)
    , _server(ip, port)
    {

    }

    ~EchoServer()
    {
    }

    void start()
    {
        _pool.start();
        /* _server.setAllCallback(std::move(onNewConnection) */
        /*                        , std::move(onMessage) */
        /*                        , std::move(onClose)); */
        using namespace std::placeholders;
        _server.setAllCallback(std::bind(&EchoServer::onNewConnection, this, _1)
                               , std::bind(&EchoServer::onMessage, this, _1)
                               , std::bind(&EchoServer::onClose, this, _1));
        _server.start();
    }
    void stop()
    {
        _pool.stop();
        _server.stop();
    }


private:
    //1、连接建立做的事件
    void onNewConnection(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has connected!" << endl;
    }
    
    //2、消息到达做的事件
    void onMessage(const TcpConnectionPtr &con)
    {
        string msg = con->receive();//接收客户端的数据
        cout << ">>recv msg from client " << msg << endl;
    
        //接收到客户端的msg之后，是可以进行业务逻辑的处理
        MyTask task(msg, con);
        //添加了Task，添加到了线程池里面去了,执行了_taskQue
        //中的push
        _pool.addTask(std::bind(&MyTask::process, task));
    }
    
    //3、连接断开做的事件
    void onClose(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has closed!" << endl;
    }

private:
    ThreadPool _pool;
    TcpServer _server;
};

#endif
