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
@brief          ABH3用CAN サンプルソフト(アプリケーション)
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


//読み込むDLLファイル（CanA31プロジェクトの出力）
#define DLLFILE		_T("CanA31.dll")

//設定要素
#define USEIF		(0)		//0..IxxatV2デバイスを使用	1..IxxatSimpleデバイスを使用
#define IFNUM		(0)		//IxxatV2の場合何本目か(0開始)、IxxatSimpleの場合COMポート番号(1開始)
#define TGTID		(1)		//ターゲット(ABH3)側で使用するID
#define	HOSTID		(2)		//ホスト(PC)側で使用するID
#define	OPENMS		(1000)	//回線を開く場合のタイムアウト値[ms]
#define SENDMS		(1000)	//送信する場合のタイムアウト値[ms]
#define RECVMS		(1000)	//受信する場合のタイムアウト値[ms]
#define	BAUDRATE	(500)	//CAN通信に使用するボーレート[kbps]
#define BRDGRP		(5)		//ブロードキャストグループ番号

//DLLの関数を簡単に使う為の構造体
CA3_FUNCLIST funcList;

//
int main()
	{
	//戻り値
	int nResult = 0;

	//========================================
	//動作環境依存要素
	//========================================

	//コンソールに日本語表示する為に必要な設定
	::_tsetlocale(LC_ALL,_T(""));

	//DLL読み込み
	HMODULE hDLL = ::LoadLibrary(DLLFILE);
	if (hDLL == NULL)
		{
		::_tprintf(_T("異常：DLLが読み込めません\n"));
		return(1);
		}

	//DLLの関数を楽に扱う為、用意された構造体にエントリを一括設定
	GetFunctions(hDLL,&funcList);

	//========================================
	//動作テスト
	//========================================

	//DLLの使い始めに1回だけ呼び出す関数
	funcList.InitInstance(USEIF);

	//インターフェースが使用するボーレートを設定
	funcList.SetBaudrate(BAUDRATE);
	::_tprintf(_T("情報：ボーレートを設定しました\n\n"));

	//送信データと受信データの格納先
	CANABH3_RESULT res;

	//処理開始
	do
		{
		//通信のタイムアウト時間を設定
		funcList.SetOpenTimeout(OPENMS);
		funcList.SetSendTimeout(SENDMS);
		funcList.SetRecvTimeout(RECVMS);
		::_tprintf(_T("情報：許容時間を設定しました\n\n"));

		//ホスト(PC)と制御対象(ABH3)のIDを設定
		funcList.SetHostID(HOSTID);
		funcList.SetTargetID(TGTID);
		::_tprintf(_T("情報：ホストとターゲットのIDを設定しました\n\n"));

		//CANインターフェースを開く
		int32_t nResult = funcList.OpenInterface(IFNUM);
		if (nResult)
			{
			::_tprintf(_T("異常：インターフェースを開く時に異常(%d)が発生しました\n\n"),nResult);
			break;
			}
		else
			::_tprintf(_T("情報：インターフェースを正常に開きました\n\n"));

		//指令の初期化
		nResult = funcList.abh3_can_init();
		if (nResult)
			{
			::_tprintf(_T("異常：初期設定コマンド(abh3_can_init)が失敗(%d)しました\n\n"),nResult);
			break;
			}
		else
			::_tprintf(_T("情報：初期設定コマンド(abh3_can_init)が成功しました\n\n"));

		//エラー解除
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

		// サーボON
		funcList.abh3_can_inSet(0x00003003,0xffffffff,&res);
		for (int i = 0; i < 1000; i++)
			{
			// 指令
			nResult = funcList.abh3_can_cmd(funcList.cnvVel2CAN(100),funcList.cnvVel2CAN(-50),&res);
			::_tprintf(_T("%10.3f %10.3f %10.3f %10.3f\n")
				,funcList.cnvCAN2Vel(res.u.DP0R.nBackSpeedY)
				,funcList.cnvCAN2Vel(res.u.DP0R.nBackSpeedX)
				,funcList.cnvCAN2Vel(res.u.DP0R.nBackSpeedA)
				,funcList.cnvCAN2Vel(res.u.DP0R.nBackSpeedB)
				);
			Sleep(1);
			}

		// サーボOFF
		funcList.abh3_can_inSet(0x00000000, 0xffffffff, &res);

		break;
		} while (1);

	//通信回線を閉じる
	funcList.CloseInterface();

	//終了処理
	funcList.ExitInstance();

	//========================================
	//動作環境依存要素
	//========================================

	//DLL開放
	::FreeLibrary(hDLL);

	//終了
	return nResult;
	}

