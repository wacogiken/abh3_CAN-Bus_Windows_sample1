
#include "stdafx.h"
#include <conio.h>
#include <locale.h>
#include "CANa31.h"
#include "typedef.h"
#include "CANa31dll.h"


//�ǂݍ���DLL�t�@�C���iCanA31�v���W�F�N�g�̏o�́j
#define DLLFILE		_T("CanA31.dll")

//�ݒ�v�f
#define USEIF		(0)		//0..IxxatV2�f�o�C�X���g�p	1..IxxatSimple�f�o�C�X���g�p
#define IFNUM		(0)		//IxxatV2�̏ꍇ���{�ڂ�(0�J�n)�AIxxatSimple�̏ꍇCOM�|�[�g�ԍ�(1�J�n)
#define TGTID		(1)		//�^�[�Q�b�g(ABH3)���Ŏg�p����ID
#define	HOSTID		(2)		//�z�X�g(PC)���Ŏg�p����ID
#define	OPENMS		(1000)	//������J���ꍇ�̃^�C���A�E�g�l[ms]
#define SENDMS		(1000)	//���M����ꍇ�̃^�C���A�E�g�l[ms]
#define RECVMS		(1000)	//��M����ꍇ�̃^�C���A�E�g�l[ms]
#define	BAUDRATE	(500)	//CAN�ʐM�Ɏg�p����{�[���[�g[kbps]
#define BRDGRP		(5)		//�u���[�h�L���X�g�O���[�v�ԍ�

int main()
	{
	//�߂�l
	int nResult = 0;

	//========================================
	//������ˑ��v�f
	//========================================

	//�R���\�[���ɓ��{��\������ׂɕK�v�Ȑݒ�
	::_tsetlocale(LC_ALL,_T(""));

	//DLL�ǂݍ���
	HMODULE hDLL = ::LoadLibrary(DLLFILE);
	if (hDLL == NULL)
		{
		::_tprintf(_T("�ُ�FDLL���ǂݍ��߂܂���\n"));
		return(1);
		}

	//DLL�̊֐����y�Ɉ����ׁA�p�ӂ��ꂽ�\���̂ɃG���g�����ꊇ�ݒ�
	CA3_FUNCLIST funcList;
	GetFunctions(hDLL,&funcList);

	//========================================
	//����e�X�g
	//========================================

	//DLL�̎g���n�߂�1�񂾂��Ăяo���֐�
	funcList.InitInstance(USEIF);

	//�C���^�[�t�F�[�X���g�p����{�[���[�g��ݒ�
	funcList.SetBaudrate(BAUDRATE);
	::_tprintf(_T("���F�{�[���[�g��ݒ肵�܂���\n\n"));

	//���M�f�[�^�Ǝ�M�f�[�^�̊i�[��
	CANABH3_RESULT res;

	//�����J�n
	do
		{
		//�ʐM�̃^�C���A�E�g���Ԃ�ݒ�
		funcList.SetOpenTimeout(OPENMS);
		funcList.SetSendTimeout(SENDMS);
		funcList.SetRecvTimeout(RECVMS);
		::_tprintf(_T("���F���e���Ԃ�ݒ肵�܂���\n\n"));

		//�z�X�g(PC)�Ɛ���Ώ�(ABH3)��ID��ݒ�
		funcList.SetHostID(HOSTID);
		funcList.SetTargetID(TGTID);
		::_tprintf(_T("���F�z�X�g�ƃ^�[�Q�b�g��ID��ݒ肵�܂���\n\n"));

		//CAN�C���^�[�t�F�[�X���J��
		int32_t nResult = funcList.OpenInterface(IFNUM);
		if (nResult)
			{
			::_tprintf(_T("�ُ�F�C���^�[�t�F�[�X���J�����Ɉُ�(%d)���������܂���\n\n"),nResult);
			break;
			}
		else
			::_tprintf(_T("���F�C���^�[�t�F�[�X�𐳏�ɊJ���܂���\n\n"));

		//�w�߂̏�����
		nResult = funcList.abh3_can_init();
		if (nResult)
			{
			::_tprintf(_T("�ُ�F�����ݒ�R�}���h(abh3_can_init)�����s(%d)���܂���\n\n"),nResult);
			break;
			}
		else
			::_tprintf(_T("���F�����ݒ�R�}���h(abh3_can_init)���������܂���\n\n"));

		//�G���[����
		funcList.abh3_can_inSet(0x80000000,0xffffffff,&res);
		while (1)
			{
			funcList.abh3_can_reqBRD(BRDGRP * 8 + 0,&res);
			::_tprintf(_T("Abnormal: %08x  Warning: %08x\n")
				,res.u.BR0.nErrorBit
				,res.u.BR0.nWarnBit
				);
			if (res.u.BR0.nErrorBit)
				{
				funcList.abh3_can_inSet(0x80000000,0xffffffff,&res);
				}
			else
				{
				break;
				}
			}
		funcList.abh3_can_inSet(0x00000000,0xffffffff,&res);

		// �T�[�{ON
		funcList.abh3_can_inSet(0x00003003,0xffffffff,&res);
		for (int i = 0; i < 1000; i++)
			{
			// �w��
			nResult = funcList.abh3_can_cmd(funcList.cnvVel2CAN(100),funcList.cnvVel2CAN(-50),&res);
			::_tprintf(_T("%10.3f %10.3f %10.3f %10.3f\n")
				,funcList.cnvCAN2Vel(res.u.DP0R.nBackSpeedY)
				,funcList.cnvCAN2Vel(res.u.DP0R.nBackSpeedX)
				,funcList.cnvCAN2Vel(res.u.DP0R.nBackSpeedA)
				,funcList.cnvCAN2Vel(res.u.DP0R.nBackSpeedB)
				);
			Sleep(1);
			}

		// �T�[�{OFF
		funcList.abh3_can_inSet(0x00000000, 0xffffffff, &res);

		break;
		} while (1);

	//�ʐM��������
	funcList.CloseInterface();

	//�I������
	funcList.ExitInstance();

	//========================================
	//������ˑ��v�f
	//========================================

	//DLL�J��
	::FreeLibrary(hDLL);

	//�I��
	return nResult;
	}

