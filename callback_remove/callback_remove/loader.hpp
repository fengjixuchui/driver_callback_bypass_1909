#pragma once
#include <Windows.h>

#include <iostream>

/* ��װ���� */
unsigned long
install_driver(
	const wchar_t* drvPath,
	const wchar_t* serviceName)
{
	// �򿪷�����ƹ��������ݿ�
	SC_HANDLE schSCManager = OpenSCManagerW(
		NULL,												// Ŀ������������,NULL�����ӱ��ؼ�����ϵķ�����ƹ�����
		NULL,												// ������ƹ��������ݿ�����ƣ�NULL���� SERVICES_ACTIVE_DATABASE ���ݿ�
		SC_MANAGER_ALL_ACCESS);			// ����Ȩ��
	if (schSCManager == NULL)
		return GetLastError();

	// ����������������������ƹ��������ݿ�
	SC_HANDLE schService = CreateServiceW(
		schSCManager,									// ����ؼ����������ݿ�ľ��
		serviceName,										// Ҫ��װ�ķ��������
		serviceName,										// �û��������������ʶ�������ʾ����
		SERVICE_ALL_ACCESS,							// �Է���ķ���Ȩ�ޣ�����ȫȨ��
		SERVICE_KERNEL_DRIVER,					// �������ͣ���������
		SERVICE_DEMAND_START,					// ��������ѡ����̵��� StartService ʱ����
		SERVICE_ERROR_IGNORE,					// ����޷����������Դ����������
		drvPath,												// �����ļ�����·������������ո���Ҫ���˫����
		NULL,													// ���������ĸ��ض����飺��������ĳ����
		NULL,													// ���ն�����Ψһ���ֵ��������
		NULL,													// �������˳�����飺����û��������
		NULL,													// ���з�����˻�����ʹ�� LocalSystem �˻�
		NULL);													// LocalSystem �˻�����
	if (schService == NULL)
	{
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return 0;
}

/* �������� */
unsigned long
start_driver(
	const wchar_t* serviceName)
{
	// �򿪷�����ƹ��������ݿ�
	SC_HANDLE schSCManager = OpenSCManagerW(
		NULL,													// Ŀ������������,NULL�����ӱ��ؼ�����ϵķ�����ƹ�����
		NULL,													// ������ƹ��������ݿ�����ƣ�NULL���� SERVICES_ACTIVE_DATABASE ���ݿ�
		SC_MANAGER_ALL_ACCESS);				// ����Ȩ��
	if (schSCManager == NULL)
		return GetLastError();

	// �򿪷���
	SC_HANDLE hs = OpenServiceW(
		schSCManager,						// ����ؼ����������ݿ�ľ��
		serviceName,							// Ҫ�򿪵ķ�����
		SERVICE_ALL_ACCESS);			// �������Ȩ�ޣ�����Ȩ��
	if (hs == NULL)
	{
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	// ��ʼ����
	if (StartServiceW(hs, 0, 0) == 0)
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);

	return 0;
}

/* ֹͣ���� */
unsigned long
stop_driver(
	const wchar_t* serviceName)
{
	// �򿪷�����ƹ��������ݿ�
	SC_HANDLE schSCManager = OpenSCManagerW(
		NULL,												// Ŀ������������,NULL�����ӱ��ؼ�����ϵķ�����ƹ�����
		NULL,												// ������ƹ��������ݿ�����ƣ�NULL���� SERVICES_ACTIVE_DATABASE ���ݿ�
		SC_MANAGER_ALL_ACCESS);			// ����Ȩ��
	if (schSCManager == NULL)
		return GetLastError();

	// �򿪷���
	SC_HANDLE hs = OpenServiceW(
		schSCManager,							// ����ؼ����������ݿ�ľ��
		serviceName,								// Ҫ�򿪵ķ�����
		SERVICE_ALL_ACCESS);				// �������Ȩ�ޣ�����Ȩ��
	if (hs == NULL)
	{
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	SERVICE_STATUS status;
	if (QueryServiceStatus(hs, &status) == 0)
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	if (status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING)
	{
		// ���͹رշ�������
		if (ControlService(
			hs,												// ������
			SERVICE_CONTROL_STOP,			// �����룺֪ͨ����Ӧ��ֹͣ
			&status										// �������µķ���״̬��Ϣ
		) == 0)
		{
			CloseServiceHandle(hs);
			CloseServiceHandle(schSCManager);
			return GetLastError();
		}

		// �жϳ�ʱ
		int timeOut = 0;
		while (status.dwCurrentState != SERVICE_STOPPED)
		{
			timeOut++;
			QueryServiceStatus(hs, &status);
			Sleep(50);
		}

		if (timeOut > 80)
		{
			CloseServiceHandle(hs);
			CloseServiceHandle(schSCManager);
			return GetLastError();
		}

		return 0;
	}

	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return true;
}

/* ж������ */
unsigned long
unload_driver(
	const wchar_t* serviceName)
{
	// �򿪷�����ƹ��������ݿ�
	SC_HANDLE schSCManager = OpenSCManagerW(
		NULL,												// Ŀ������������,NULL�����ӱ��ؼ�����ϵķ�����ƹ�����
		NULL,												// ������ƹ��������ݿ�����ƣ�NULL���� SERVICES_ACTIVE_DATABASE ���ݿ�
		SC_MANAGER_ALL_ACCESS);			// ����Ȩ��
	if (schSCManager == NULL)
		return GetLastError();

	// �򿪷���
	SC_HANDLE hs = OpenServiceW(
		schSCManager,						// ����ؼ����������ݿ�ľ��
		serviceName,							// Ҫ�򿪵ķ�����
		SERVICE_ALL_ACCESS);			// �������Ȩ�ޣ�����Ȩ��
	if (hs == NULL)
	{
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	// ɾ������
	if (DeleteService(hs) == 0)
	{
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);

	return 0;
}

/* ��ʼж�� */
bool
start_uninstall_driver(
	const wchar_t* serviceName)
{
	stop_driver(serviceName);
	return unload_driver(serviceName) == 0;
}

/* ��ʼ��װ */
bool
start_install_driver(
	const wchar_t* drvPath,
	const wchar_t* serviceName,
	bool uninstall = false)
{
	// �ȳ���ж��
	if (uninstall)
		start_uninstall_driver(serviceName);

	// ���Լ���
	install_driver(drvPath, serviceName);
	return start_driver(serviceName) == 0;
}