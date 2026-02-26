#include "../ThirdPartyHeader/httplib.h"

#include "Player/Path.h"
#include "Player/Player.h"

#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <codecvt>
#include <fstream>
#include <sstream>

Path Paths = Path(
	{
		{842,842},{1322,842},{1322,442},
		{2762,442},{2762,842},{3162,842},
		{3162,1722},{2122,1722},{2122,1562},
		{842,1562},{842,842}
	}
); //玩家移动路径


enum class Stage
{
	Waiting, //等待玩家进入
	Ready, //准备起跑倒计时
	Racing //正在比赛中
};

int ValCountdown = 4; //起跑倒计时
Stage Stageing = Stage::Waiting; //当前游戏阶段

int IdPlayer = 0; //玩家序号
std::atomic<int> Progress1 = -1; //玩家1进度
std::atomic<int> Progress2 = -1; //玩家2进度
int	NumTotalChar = 0; // 全部字符数


int IdxLine = 0; //当前文本行索引
int IdxChar = 0; //当前文本字符索引
std::string TextContent; //文本内容
std::vector<std::string> StrLineList; //行文件列表

//玩家1闲置动画图集
Atlas Atlas1PIdleUp;   //上
Atlas Atlas1PIdleDown; //下
Atlas Atlas1PIdleLeft; //左
Atlas Atlas1PIdleRight;//右

//玩家1奔跑动画图集
Atlas Atlas1PRunUp;  //上
Atlas Atlas1PRunDown;//下
Atlas Atlas1PRunLeft;//左
Atlas Atlas1PRunRight;//右;

//玩家2闲置动画图集
Atlas Atlas2PIdleUp;   //上
Atlas Atlas2PIdleDown; //下
Atlas Atlas2PIdleLeft; //左
Atlas Atlas2PIdleRight;//右

//玩家2奔跑动画图集
Atlas Atlas2PRunUp;  //上
Atlas Atlas2PRunDown;//下
Atlas Atlas2PRunLeft;//左
Atlas Atlas2PRunRight;//右;

IMAGE ImageUi1; //界面文本1
IMAGE ImageUi2; //界面文本2
IMAGE ImageUi3; //界面文本3
IMAGE ImageUiFight;//界面文本Fight
IMAGE ImageUiTextBox; //界面文本框
IMAGE ImageBackground; //背景图

std::string StrAddress; //服务器地址
httplib::Client* client = nullptr; //Http客户端对象


void LoadResources(HWND hwnd)
{
	AddFontResourceEx(_T("resources/Ipix.ttf"), FR_PRIVATE, NULL);

	Atlas1PIdleUp.Load(_T("resources/hajimi_idle_back_%d.png"), 4);
	Atlas1PIdleDown.Load(_T("resources/hajimi_idle_front_%d.png"), 4);
	Atlas1PIdleLeft.Load(_T("resources/hajimi_idle_left_%d.png"), 4);
	Atlas1PIdleRight.Load(_T("resources/hajimi_idle_right_%d.png"), 4);

	Atlas1PRunUp.Load(_T("resources/hajimi_run_back_%d.png"), 4);
	Atlas1PRunDown.Load(_T("resources/hajimi_run_front_%d.png"), 4);
	Atlas1PRunLeft.Load(_T("resources/hajimi_run_left_%d.png"), 4);
	Atlas1PRunRight.Load(_T("resources/hajimi_run_right_%d.png"), 4);

	Atlas2PIdleUp.Load(_T("resources/manbo_idle_back_%d.png"), 4);
	Atlas2PIdleDown.Load(_T("resources/manbo_idle_front_%d.png"), 4);
	Atlas2PIdleLeft.Load(_T("resources/manbo_idle_left_%d.png"), 4);
	Atlas2PIdleRight.Load(_T("resources/manbo_idle_right_%d.png"), 4);

	Atlas2PRunUp.Load(_T("resources/manbo_run_back_%d.png"), 4);
	Atlas2PRunDown.Load(_T("resources/manbo_run_front_%d.png"), 4);
	Atlas2PRunLeft.Load(_T("resources/manbo_run_left_%d.png"), 4);
	Atlas2PRunRight.Load(_T("resources/manbo_run_right_%d.png"), 4);

	loadimage(&ImageUi1, _T("resources/ui_1.png"));
	loadimage(&ImageUi2, _T("resources/ui_2.png"));
	loadimage(&ImageUi3, _T("resources/ui_3.png"));
	loadimage(&ImageUiFight, _T("resources/ui_fight.png"));
	loadimage(&ImageUiTextBox, _T("resources/ui_textbox.png"));
	loadimage(&ImageBackground, _T("resources/background.png"));

	LoadAudio(_T("resources/bgm.mp3"), _T("bgm"));
	LoadAudio(_T("resources/1p_win.mp3"), _T("1p_win"));
	LoadAudio(_T("resources/2p_win.mp3"), _T("2p_win"));
	LoadAudio(_T("resources/click_1.mp3"), _T("click_1"));
	LoadAudio(_T("resources/click_2.mp3"), _T("click_2"));
	LoadAudio(_T("resources/click_3.mp3"), _T("click_3"));
	LoadAudio(_T("resources/click_4.mp3"), _T("click_4"));
	LoadAudio(_T("resources/ui_1.mp3"), _T("ui_1"));
	LoadAudio(_T("resources/ui_2.mp3"), _T("ui_2"));
	LoadAudio(_T("resources/ui_3.mp3"), _T("ui_3"));
	LoadAudio(_T("resources/ui_fight.mp3"), _T("ui_fight"));

	std::ifstream file("config.cfg");

	if (!file.good())
	{
		MessageBox(hwnd, L"无法打开配置 config", L"启动失败", MB_OK | MB_ICONERROR);
		exit(-1);
	}

	std::stringstream strStream;
	strStream << file.rdbuf();
	StrAddress = strStream.str();

	file.close();
}

void LoginToServer(HWND hwnd)
{
	client = new httplib::Client(StrAddress);
	client->set_keep_alive(true);

	httplib::Result result = client->Post("/login");
	if (!result || result->status != 200)
	{
		MessageBox(hwnd, L"无法连接服务器", L"连接失败", MB_OK | MB_ICONERROR);
		exit(-1);
	}

	IdPlayer = std::stoi(result->body);

	if (IdPlayer <= 0)
	{
		MessageBox(hwnd, L"游戏已经开始了", L"拒绝加入", MB_OK | MB_ICONERROR);
		exit(-1);
	}

	(IdPlayer == 1) ? (Progress1 = 0) : (Progress2 = 0);

	TextContent = client->Post("/queryText")->body;

	//计算游戏进度
	std::stringstream strStream(TextContent);
	std::string strLine;
	while (std::getline(strStream, strLine))
	{
		StrLineList.push_back(strLine);
		NumTotalChar += (int)strLine.length();
	}

	std::thread([&]() {

		while (true)
		{
			using namespace std::chrono;

			std::string route = (IdPlayer == 1) ? "/update1" : "/update2";
			std::string body = std::to_string((IdPlayer == 1) ? Progress1 : Progress2);

			httplib::Result result = client->Post(route, body, "text/plain");

			if (result && result->status == 200)
			{
				int progress = std::stoi(result->body);
				(IdPlayer == 1) ? (Progress2 = progress) : (Progress1 = progress);
			}
			std::this_thread::sleep_for(nanoseconds(1000000000 / 10));
			
		}
		}).detach(); //detach将匿名线程与主线程分离,进行并行进行

}

int main(int argc, char** argv)
{

	//处理数据初始化
	using namespace std::chrono;

	HWND hwnd = initgraph(1280, 720);
	SetWindowText(hwnd, _T("哈基米大冒险!"));
	settextstyle(28, 0, _T("Ipix"));

	setbkmode(TRANSPARENT);

	LoadResources(hwnd);
	LoginToServer(hwnd);

	ExMessage msg;
	Timer timerCountdown;
	Camera cameraUi, cameraScene;
	Player player1(&Atlas1PIdleUp,
		&Atlas1PIdleDown,
		&Atlas1PIdleLeft,
		&Atlas1PIdleRight,
		&Atlas1PIdleUp,
		&Atlas1PRunDown,
		&Atlas1PRunLeft,
		&Atlas1PRunRight);
	Player player2(&Atlas2PIdleUp,
		&Atlas2PIdleDown,
		&Atlas2PIdleLeft,
		&Atlas2PIdleRight,
		&Atlas2PIdleUp,
		&Atlas2PRunDown,
		&Atlas2PRunLeft,
		&Atlas2PRunRight);

	cameraUi.SetSize({ 1280,720 });
	cameraScene.SetSize({ 1280,720 });


	player1.SetPosition({ 842,842 });
	player2.SetPosition({ 842,842 });

	timerCountdown.SetOneShot(false);
	timerCountdown.SetWaitTime(1.0f);
	timerCountdown.SetOnTimeout([&]()
		{
			ValCountdown--;

			switch (ValCountdown)
			{
			case 3:
				PlayAudio(_T("ui_3"));
				break;
			case 2:
				PlayAudio(_T("ui_2"));
				break;
			case 1:
				PlayAudio(_T("ui_1"));
				break;
			case 0:
				PlayAudio(_T("ui_fight"));
				break;
			case -1:
				Stageing = Stage::Racing;
				PlayAudio(_T("bgm"),true);
				break;
			}
		});

	const nanoseconds frameDuration(1000000000 / 144);
	steady_clock::time_point lastTick = steady_clock::now();
	BeginBatchDraw();

	while (true)
	{
		//玩家输入处理
		while (peekmessage(&msg))
		{
			if (Stageing != Stage::Racing)continue;

			if (msg.message == WM_CHAR && IdxLine < StrLineList.size())
			{
				const std::string& strLine = StrLineList[IdxLine];
				if (strLine[IdxChar] == msg.ch)
				{
					switch (rand() % 4)
					{
					case 0:
						PlayAudio(_T("click_1"));
						break;
					case 1:
						PlayAudio(_T("click_2"));
						break;
					case 2:
						PlayAudio(_T("click_3"));
						break;
					case 3:
						PlayAudio(_T("click_4"));
						break;
					}

					(IdPlayer == 1) ? Progress1++:Progress2++;

					IdxChar++;
					if (IdxChar >= strLine.length())
					{
						IdxChar = 0;
						IdxLine++;
					}
				}
			}
		}

		//处理游戏更新
		steady_clock::time_point frameStart = steady_clock::now();
		duration<float> delta = duration<float>(frameStart - lastTick);

		if (Stageing == Stage::Waiting)
		{
			if (Progress1 >= 0 && Progress2 >= 0)Stageing = Stage::Ready;
		}
		else
		{
			if (Stageing == Stage::Ready)timerCountdown.OnUpdate(delta.count());

			if ((IdPlayer == 1 && Progress1 >= NumTotalChar)
				|| (IdPlayer == 2 && Progress2 >= NumTotalChar))
			{
				StopAudio(_T("bgm"));
				PlayAudio((IdPlayer == 1) ? _T("1p_win") : _T("2p_win"));
				MessageBox(hwnd, _T("WIN!"), _T("游戏结束了"), MB_OK | MB_ICONINFORMATION);
				exit(0);
			}
			else if ((IdPlayer == 1 && Progress2 >= NumTotalChar)
				|| (IdPlayer == 2 && Progress1 >= NumTotalChar))
			{
				StopAudio(_T("bgm"));
				MessageBox(hwnd, _T("LOW!"), _T("游戏结束"), MB_OK | MB_ICONINFORMATION);
				exit(0);
			}

			player1.SetTarget(Paths.GetPositionAtProgress((float)Progress1 / NumTotalChar));
			player2.SetTarget(Paths.GetPositionAtProgress((float)Progress2 / NumTotalChar));

			player1.OnUpdate(delta.count());
			player2.OnUpdate(delta.count());

			cameraScene.LookAt((IdPlayer == 1)
				? player1.GetPosition() : player2.GetPosition());
		}

		//处理画面绘制
		setbkcolor(RGB(0, 0, 0));
		cleardevice();

		if (Stageing == Stage::Waiting)
		{
			settextcolor(RGB(195, 195, 195));
			outtextxy(15, 675, _T("等待其他玩家加入!"));
		}
		else
		{
			static const Rect rectBg =
			{
				0,0,
				ImageBackground.getwidth(),
				ImageBackground.getheight()
			};

			PutimageAlphaEx(cameraScene, &ImageBackground, &rectBg);

			if (player1.GetPosition().Y > player2.GetPosition().Y)
			{
				player2.OnRender(cameraScene);
				player1.OnRender(cameraScene);
			}
			else
			{
				player1.OnRender(cameraScene);
				player2.OnRender(cameraScene);	
			}

			switch (ValCountdown)
			{
			case 3:
			{
				static const Rect rectUi3 =
				{
					1280 / 2 - ImageUi3.getwidth() / 2,
					720 / 2 - ImageUi3.getheight() / 2,
					ImageUi3.getwidth(),ImageUi3.getheight()
				};
				PutimageAlphaEx(cameraUi, &ImageUi3, &rectUi3);
			}
			break;
			case 2: 
			{
				static const Rect rectUi2 =
				{
					1280 / 2 - ImageUi2.getwidth() / 2,
					720 / 2 - ImageUi2.getheight() / 2,
					ImageUi2.getwidth(),ImageUi2.getheight()
				};
				PutimageAlphaEx(cameraUi, &ImageUi2, &rectUi2);
			}
			break;
			case 1:
			{
				static const Rect rectUi1 =
				{
					1280 / 2 - ImageUi1.getwidth() / 2,
					720 / 2 - ImageUi1.getheight() / 2,
					ImageUi1.getwidth(),ImageUi1.getheight()
				};
				PutimageAlphaEx(cameraUi, &ImageUi1, &rectUi1);
			}
			break;	
			case 0:
			{
				static const Rect rectUiFight =
				{
					1280 / 2 - ImageUiFight.getwidth() / 2,
					720 / 2 - ImageUiFight.getheight() / 2,
					ImageUiFight.getwidth(),ImageUiFight.getheight()
				};
				PutimageAlphaEx(cameraUi, &ImageUiFight, &rectUiFight);
			}
			break;
			default:break;
			}

			if (Stageing == Stage::Racing)
			{
				static const Rect rectTextBox =
				{
					0,
					720 - ImageUiTextBox.getheight(),
					ImageUiTextBox.getwidth(),
					ImageUiTextBox.getheight()
				};

				/*
				* EasyX绘制文本所需参数是宽字符字符串
				* 网络传输的文本内容是utf8编码，并是string存储
				* 则需要wstring_convert进行字符串编码转换
				*/
				static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
				std::wstring wstrLine = convert.from_bytes(StrLineList[IdxLine]);
				std::wstring wstrCompleted = convert.from_bytes(StrLineList[IdxLine].substr(0, IdxChar));
				
				PutimageAlphaEx(cameraUi, &ImageUiTextBox, &rectTextBox);
				settextcolor(RGB(125, 125, 125));
				outtextxy(185 + 2, rectTextBox.Y + 65 + 2, wstrLine.c_str());
				settextcolor(RGB(25, 25, 25));
				outtextxy(185, rectTextBox.Y + 65, wstrLine.c_str());
				settextcolor(RGB(0, 149, 217));
				outtextxy(185, rectTextBox.Y + 65, wstrCompleted.c_str());
			}
		}

		FlushBatchDraw();

		lastTick = frameStart;
		nanoseconds sleepDuration = frameDuration - (steady_clock::now() - frameStart);
		if (sleepDuration > nanoseconds(0))std::this_thread::sleep_for(sleepDuration);
	}


	return 0;
}