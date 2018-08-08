#pragma once
class CResultWnd
{
public:
	CResultWnd(ResultData* rd);
	~CResultWnd();

	void MainLoop(int hGraph);

private:

	void Draw(int hGraph);
	void Update();
	void Hittest();

	ResultData m_Result;
};

