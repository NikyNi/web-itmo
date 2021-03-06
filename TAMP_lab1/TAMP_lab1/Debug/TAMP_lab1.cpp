﻿/*Лаба 1. – до ноября
Задание :Разработать программу, запрещающую в текущем(том, котором она находится) каталоге создание,
копирование или переименование файлов с заданными именами(можно использовать маски файлов),
в каталогах по уровню выше и ниже текущего эти запреты не работают.

Список имен или их шаблонов хранить в текущем каталоге в файле template.tbl, как текст.
Должна быть обеспечена защита этого файла от удаления, несанкционированного просмотра и модификации.

(По желанию) При установке программы можно предусмотреть ее отключение с использованием пароля, хранящегося в первой строке файла template.tbl в шифрованном виде.

Для ОС Windows рекомендуется использовать : SetFileSecurity и SetNamedSecurityInfo, а также другие материалы из источника :
SetFileSecurity*/
#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <stdio.h>
#include <aclapi.h> 
#include <string>
#include <fstream>
#include<TCHAR.H> 
#include<sddl.h>
#include "atlstr.h"
#ifdef UNICODE
#define SetWindowText  SetWindowTextW
#else
#define SetWindowText  SetWindowTextA
#endif 
#define A2T A2W
using namespace std;


// подключаем библиотеку ACE с готовыми классами, чтобы подключить ее к дискриптору безопасности
DWORD set_DACL_for_object(
	TCHAR *object,// имя файла, который мы открываем.LPTSTR это логический указатель на тип tchar
	SE_OBJECT_TYPE ObjectType,  // тип объекта Windows Objects 
	int access_perms,
	//int access_mode
	//LPTSTR pszTrustee,          // trustee for new ACE
	//TRUSTEE_FORM TrusteeForm,   // format of trustee structure
	//DWORD dwAccessRights,       // access mask for new ACE
	ACCESS_MODE AccessMode     // устанавливает тип права доступа (разрешить, запретить доступ)
	//DWORD dwInheritance         // inheritance flags for new ACE
)
{
	//DWORD dwRes = 0;
	PACL pDACL = NULL, pOldDACL = NULL;// pacl указатель на acl структуру, в которую входит dacl. 
	PSECURITY_DESCRIPTOR pSD = NULL;// указатель на дескриптор безопасности
	PSID pAdminSID = NULL;
	ConvertStringSidToSid(L"S-1-5-21-65355594-2071056060-2550055772-1001", &pAdminSID);// мой sid как админа
	EXPLICIT_ACCESS ea; // устанавливает право доступа для файла
//	if (file == NULL)
	//	return ERROR_INVALID_PARAMETER;
	// Получаем указатель на текущий DACL. : SetFileSecurity
	DWORD Res1 = GetNamedSecurityInfo((LPTSTR)object, ObjectType,
		DACL_SECURITY_INFORMATION,
		NULL, NULL, &pOldDACL, NULL, &pSD);
	if (ERROR_SUCCESS != Res1) {
		cout << "GetNamedSecurityInfo Error" << Res1;
	}
	else {
		cout << "GetNamedSecurityInfo Done"<< endl;
	}
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	// Создаем EXPLICIT_ACCESS структуру управления доступом для АСЕ
	ea.grfAccessPermissions = access_perms; // регулирует права доступа 
	ea.grfAccessMode = AccessMode;// разрешено ли право доступа?
	ea.grfInheritance = NO_INHERITANCE; // отмечает наследуется ли признак. Мы его не наследуем
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID; // как мы определяем имя пользователя, для которого устанавливаем права доступа. Мы делаем это по SID 
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.ptstrName = (LPTSTR)pAdminSID;// указатель на  SID пользователя

	// Create a new ACL that merges the new ACE
	// into the existing DACL.
	DWORD Res = SetEntriesInAcl(1, &ea, pOldDACL, &pDACL);
	if (ERROR_SUCCESS != Res) {
		cout << "SetEntriesInAcl Error" <<Res;}
	// Attach the new ACL as the object's DACL.
   // DWORD dwRes = 0;
	DWORD dwRes = SetNamedSecurityInfo((LPTSTR)object, ObjectType,
		DACL_SECURITY_INFORMATION,
		NULL, NULL, pDACL, NULL);
	if (ERROR_SUCCESS != dwRes) {
		cout <<"SetNamedSecurityInfo Error"<< dwRes;
	}
	else {
		cout << "SetNamedSecurityInfo Done" << dwRes <<endl;
	}
	return dwRes;
}
int _tmain(int argc, TCHAR *argv[]) {
	setlocale(LC_ALL, "rus");
	wstring line;
	//считали исходник
	char path2[] = "C:\\Users\\Ninel\\Desktop\\study\\programbe\\TAMP_lab1\\TAMP_lab1\\template.doc";
	USES_CONVERSION;
	TCHAR* path22 = A2T(path2);
	set_DACL_for_object(path22, SE_FILE_OBJECT, GENERIC_ALL, SET_ACCESS);
	//set_DACL_for_object(path22, SE_FILE_OBJECT, GENERIC_READ, SET_ACCESS);
	wifstream fin(path2);
	if (!fin.is_open()) { // если файл не открыт
		cout << "Файл не открыт!\n";
	}
	else {
		//cout << "OK" << endl;
		int k = 0;
		while (getline(fin, line)) {
			k += 1;
			cout << k << endl;
			wcout << line << endl;
			//вызывать функцию, где устанавливается право на то то, то то
		}
		fin.close();
	}
	return 0;
};

