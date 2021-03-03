/*
 ******************************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2021, Waco Giken Co., Ltd.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/o2r other materials provided
 *     with the distribution.
 *   * Neither the name of the Waco Giken nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************

******************************************************************************
@file           single_canABH3.cpp
******************************************************************************
@brief          ABH3�pCAN �T���v���\�t�g(�A�v���P�[�V����)
******************************************************************************
@date           2021.03.03
@author         Y.Ogawa
@note			1st release
@note           This sourcecode is based by "single_canABH3.c"
******************************************************************************
*/

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

//DLL�̊֐����ȒP�Ɏg���ׂ̍\����
CA3_FUNCLIST funcList;

//
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

