#include <Eigen/Eigen>

#include <aris.h>
#include <Robot_Type_I.h>


Robots::RobotTypeI rbt;

struct SimpleWalkParam_Aris final :public Aris::Dynamic::PlanParamBase
{
	double beginPee[18]{ 0 };
	double beginPeb[6]{ 0 };
	std::int32_t totalCount{ 500 };
	std::int32_t n{ 1 };
	double d{ 0.5 };
	double h{ 0.05 };
};
int SimpleWalk_Aris(Aris::Dynamic::ModelBase &model, const Aris::Dynamic::PlanParamBase & param)
{
	auto &sp = static_cast<const SimpleWalkParam_Aris&>(param);
	auto &robot = static_cast<Robots::RobotBase&>(model);

	static Aris::Dynamic::FloatMarker beginBodyMak(robot.Ground(), nullptr, "313");
	static double beginEE[18];

	/*在每次脚着地时更新与身体坐标系重合的位于地面的坐标系*/
	if ((sp.count % sp.totalCount) == 0)
	{
		beginBodyMak.setPrtPm(*robot.Body().pm());
		beginBodyMak.update();
		robot.GetPee(beginEE, beginBodyMak);
	}

	double pEE[18], pe[6]{ 0 };
	std::copy_n(beginEE, 18, pEE);

	/*当前相位的count*/
	int count = sp.count % sp.totalCount;

	if ((sp.count / sp.totalCount) == 0)/*加速段*/
	{
		pe[2] = Aris::Dynamic::acc_even(sp.totalCount, count + 1)*0.25*sp.d;

		double s = -(PI / 2)*cos(PI * (count + 1) / sp.totalCount) + PI / 2;

		for (int i = 0; i < 18; i += 6)
		{
			pEE[i + 1] = sp.h*sin(s) + beginEE[i + 1];
			pEE[i + 2] = sp.d / 2 * (1 - cos(s)) / 2 + beginEE[i + 2];
		}
	}
	else if ((sp.count / sp.totalCount) == (sp.n * 2 - 1))/*减速段*/
	{
		pe[2] = Aris::Dynamic::dec_even(sp.totalCount, count + 1)*0.25*sp.d;

		double s = -(PI / 2)*cos(PI * (count + 1) / sp.totalCount) + PI / 2;

		for (int i = 3; i < 18; i += 6)
		{
			pEE[i + 1] = sp.h*sin(s) + beginEE[i + 1];
			pEE[i + 2] = sp.d / 2 * (1 - cos(s)) / 2 + beginEE[i + 2];
		}
	}
	else if ((sp.count / sp.totalCount) % 2 == 1)/*第一匀速段，紧接着加速段*/
	{
		pe[2] = Aris::Dynamic::even(sp.totalCount, count + 1)*0.5*sp.d;

		double s = -(PI / 2)*cos(PI * (count + 1) / sp.totalCount) + PI / 2;

		for (int i = 3; i < 18; i += 6)
		{
			pEE[i + 1] = sp.h*sin(s) + beginEE[i + 1];
			pEE[i + 2] = sp.d * (1 - cos(s)) / 2 + beginEE[i + 2];
		}
	}
	else/*第二匀速段，后面就是减速段*/
	{
		pe[2] = Aris::Dynamic::even(sp.totalCount, count + 1)*0.5*sp.d;

		double s = -(PI / 2)*cos(PI * (count + 1) / sp.totalCount) + PI / 2;

		for (int i = 0; i < 18; i += 6)
		{
			pEE[i + 1] = sp.h*sin(s) + beginEE[i + 1];
			pEE[i + 2] = sp.d * (1 - cos(s)) / 2 + beginEE[i + 2];
		}
	}

	robot.SetPeb(pe, beginBodyMak);
	robot.SetPee(pEE, beginBodyMak);

	return 2 * sp.n * sp.totalCount - sp.count - 1;
}

struct SimpleWalkParam final :public Robots::GaitParamBase
{
	std::int32_t totalCount{ 500 };
	std::int32_t n{ 1 };
	double d{ 0.5 };
	double h{ 0.05 };
};
int SimpleWalk(Robots::RobotBase * pRobot, const Robots::GaitParamBase * pParam)
{
	auto pSP = static_cast<const SimpleWalkParam*>(pParam);

	static Aris::Dynamic::FloatMarker beginBodyMak(pRobot->Ground(), nullptr, "313");
	static double beginEE[18];

	/*在每次脚着地时更新与身体坐标系重合的位于地面的坐标系*/
	if ((pSP->count % pSP->totalCount) == 0)
	{
		beginBodyMak.setPrtPm(*pRobot->Body().pm());
		beginBodyMak.update();
		pRobot->GetPee(beginEE, beginBodyMak);
	}

	double pEE[18], pe[6]{ 0 };
	std::copy_n(beginEE, 18, pEE);

	/*当前相位的count*/
	int count = pSP->count % pSP->totalCount;

	if ((pSP->count / pSP->totalCount) == 0)/*加速段*/
	{
		pe[2] = Aris::Dynamic::acc_even(pSP->totalCount, count + 1)*0.25*pSP->d;

		double s = -(PI / 2)*cos(PI * (count + 1) / pSP->totalCount) + PI / 2;

		for (int i = 0; i < 18; i += 6)
		{
			pEE[i + 1] = pSP->h*sin(s) + beginEE[i + 1];
			pEE[i + 2] = pSP->d / 2 * (1 - cos(s)) / 2 + beginEE[i + 2];
		}
	}
	else if ((pSP->count / pSP->totalCount) == (pSP->n * 2 - 1))/*减速段*/
	{
		pe[2] = Aris::Dynamic::dec_even(pSP->totalCount, count + 1)*0.25*pSP->d;

		double s = -(PI / 2)*cos(PI * (count + 1) / pSP->totalCount) + PI / 2;

		for (int i = 3; i < 18; i += 6)
		{
			pEE[i + 1] = pSP->h*sin(s) + beginEE[i + 1];
			pEE[i + 2] = pSP->d / 2 * (1 - cos(s)) / 2 + beginEE[i + 2];
		}
	}
	else if ((pSP->count / pSP->totalCount) % 2 == 1)/*第一匀速段，紧接着加速段*/
	{
		pe[2] = Aris::Dynamic::even(pSP->totalCount, count + 1)*0.5*pSP->d;

		double s = -(PI / 2)*cos(PI * (count + 1) / pSP->totalCount) + PI / 2;

		for (int i = 3; i < 18; i += 6)
		{
			pEE[i + 1] = pSP->h*sin(s) + beginEE[i + 1];
			pEE[i + 2] = pSP->d * (1 - cos(s)) / 2 + beginEE[i + 2];
		}
	}
	else/*第二匀速段，后面就是减速段*/
	{
		pe[2] = Aris::Dynamic::even(pSP->totalCount, count + 1)*0.5*pSP->d;

		double s = -(PI / 2)*cos(PI * (count + 1) / pSP->totalCount) + PI / 2;

		for (int i = 0; i < 18; i += 6)
		{
			pEE[i + 1] = pSP->h*sin(s) + beginEE[i + 1];
			pEE[i + 2] = pSP->d * (1 - cos(s)) / 2 + beginEE[i + 2];
		}
	}

	pRobot->SetPeb(pe, beginBodyMak);
	pRobot->SetPee(pEE, beginBodyMak);

	return 2 * pSP->n * pSP->totalCount - pSP->count - 1;
}

int main(int argc, char *argv[])
{
#ifdef WIN32
	rbt.loadXml("C:\\Robots\\resource\\Robot_Type_I\\Robot_III\\Robot_III.xml");
#endif
#ifdef UNIX
	rbt.loadXml("/usr/Robots/resource/Robot_Type_I/Robot_III/Robot_III.xml");
#endif

	const double beginEE[]{
		-0.3, -0.75, -0.65,
		-0.45, -0.75, 0,
		-0.3, -0.75, 0.65,
		0.3, -0.75, -0.65,
		0.45, -0.75, 0,
		0.3, -0.75, 0.65 };

	double beginPE[6]{0};

	rbt.SetPeb(beginPE);
	rbt.SetPee(beginEE, rbt.Body());
	
	SimpleWalkParam_Aris param;
	rbt.GetPee(param.beginPee);
	rbt.GetPeb(param.beginPeb);

	rbt.SimScriptClear();
	rbt.SimScriptSetTopologyA();
	rbt.SimScriptSimulate(500, 10);
	rbt.SimScriptSetTopologyB();
	rbt.SimScriptSimulate(500, 10);

	
	rbt.SetPeb(beginPE);
	rbt.SetPee(beginEE);
	rbt.Model::simToAdams("C:\\Users\\yang\\Desktop\\test.cmd", SimpleWalk_Aris, param, 10, true);
	
	Aris::Dynamic::SimResult result;
	rbt.SetPeb(beginPE);
	rbt.SetPee(beginEE);
	rbt.Model::simDynAkima(SimpleWalk_Aris, param, result, 10, true);
	result.saveToTxt("C:\\Users\\yang\\Desktop\\test");



	/*
	SimpleWalkParam param;
	rbt.GetPee(param.beginPee);
	rbt.GetPeb(param.beginPeb);

	rbt.SimByMatlab("C:\\Users\\yang\\Desktop\\test\\", SimpleWalk, param);
	
	rbt.SimScriptClear();
	rbt.SimScriptSetTopologyA();
	rbt.SimScriptSimulate(500, 10);
	rbt.SimScriptSetTopologyB();
	rbt.SimScriptSimulate(500, 10);
	rbt.SimByAdams("C:\\Users\\yang\\Desktop\\test.cmd", SimpleWalk, param, 10, true);
	rbt.SimByAdamsResultAt(101);

	double fin[18];
	rbt.GetFinDyn(fin);
	Aris::Dynamic::dsp(fin, 18, 1);

	{
		rbt.ClbPre();
		rbt.ClbUpd();
		rbt.ClbSetInverseMethod([](int n, double *A)
		{
			Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> >Am(A, n, n);
			auto im = Am.inverse();
			Am = im;
		});
		rbt.ForEachMotion([](Aris::Dynamic::MotionBase *p) 
		{
			p->SetMotFce(p->MotFce());
		});
		
		Aris::Dynamic::Matrix D(rbt.ClbDimM(), rbt.ClbDimN());
		Aris::Dynamic::Matrix b(rbt.ClbDimM(), 1);
		Aris::Dynamic::Matrix x(rbt.ClbDimN(), 1);

		rbt.ClbMtx(D.Data(), b.Data());
		rbt.ClbUkn(x.Data());

		auto result = D*x;
		result.dsp();
		b.dsp();
	}
	
	*/
	std::cout << "finished" << std::endl;

	char aaa;
	std::cin >> aaa;
	return 0;
}