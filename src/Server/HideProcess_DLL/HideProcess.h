#pragma once
#ifndef HIDE_PROCESS_H
#define HIDE_PROCESS_H

#include <Windows.h>
#include <Winternl.h>
#include <fstream>

typedef NTSTATUS(*typedef_ZwQuerySystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	);


NTSTATUS New_ZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
);

void HookApi();

void UnhookApi();

#endif