#include "../ThirdPartyHeader/httplib.h"


#include <mutex>
#include <string>
#include <fstream>

std::mutex GlobalMutex;  //全局互斥锁
std::string TextContent;     //文本内容
std::string TextPath = "text.txt"; //文本文件路径

int Progress1 = -1;   //玩家1进度，未加入前-1，加入后0
int Progress2 = -1;   //玩家2进度，未加入前-1，加入后0



int main(int argc,char** argv)
{
	std::ifstream loadFile(TextPath); //文本输入流

	if (!loadFile.good())
	{
		MessageBox(nullptr, L"无法打开文本文件", L"启动失败", MB_OK | MB_ICONERROR);
		return -1;
	}

	std::stringstream strStream; //获取文件内容

	strStream << loadFile.rdbuf();
	TextContent = strStream.str();

	loadFile.close();

	httplib::Server server;

	server.Post("/login", [&](const httplib::Request& req, httplib::Response& res)
		{
			std::lock_guard<std::mutex> lock(GlobalMutex);

			if (Progress1 >= 0 && Progress2 >= 0)
			{
				res.set_content("-1", "text/plain");
				return ;
			}

			res.set_content(Progress1>=0?"2":"1","text/plain");
			(Progress1 >= 0) ? (Progress2 = 0) : (Progress1 = 0);
		});

	server.Post("/queryText", [&](const httplib::Request& req, httplib::Response& res)
		{
			res.set_content(TextContent, "text/plain");
		});

	server.Post("/update1", [&](const httplib::Request& req, httplib::Response& res)
		{
			std::lock_guard<std::mutex> lock(GlobalMutex);

			Progress1 = std::stoi(req.body);
			res.set_content(std::to_string(Progress2), "text/plain");
		});

	server.Post("/update2", [&](const httplib::Request& req, httplib::Response& res)
		{
			std::lock_guard<std::mutex> lock(GlobalMutex);
			Progress2 = std::stoi(req.body);
			res.set_content(std::to_string(Progress1), "text/plain");
		});

	server.listen("0.0.0.0", 25565);
	return 0;

}