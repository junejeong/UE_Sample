// @Jung June


#include "SPSocketWorker.h"

#include <SocketSubsystem.h>
#include <Interfaces/IPv4/IPv4Address.h>
#include <IPAddress.h>
#include <Sockets.h>
#include <Async/Async.h>

FSPSocketWorker::FSPSocketWorker()
{
}

FSPSocketWorker::~FSPSocketWorker()
{
	AsyncTask(ENamedThreads::GameThread,
		[]()
		{
			// thread destroy
		});

	FSPSocketWorker::Stop();
	RemoveThread();
}

bool FSPSocketWorker::Init()
{
	bRun = false;
	bConnected = false;

	return true;
}

void FSPSocketWorker::Start()
{
	if (Thread.IsValid())
	{
		RemoveThread();
	}

	if (IpAddress.IsEmpty())
	{
		IpAddress = TEXT("127.0.0.1");
		Port = 9999;
	}

	auto* SocketThread = FRunnableThread::Create(this, *FString::Printf(TEXT("FSPSocketWorker %s:%d"), *IpAddress, Port, 0, TPri_Normal));

	Thread.Reset();
}

bool FSPSocketWorker::CreateSocket()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateUniqueSocket(NAME_Stream, TEXT("GameSocket"), false); ECPSocketStatus Status = Socket.IsValid() ? ECPSocketStatus::ConnectionReady : ECPSocketStatus::CreationFailed;
	AsyncTask(ENamedThreads::GameThread,
		[=]()
		{
			//create socket
		});

	if (Status == ECPSocketStatus::CreationFailed)
	{
		return false;
	}

	Socket->SetReceiveBufferSize(RecvBufferSize, ActualRecvBufferSize);
	Socket->SetSendBufferSize(SendBufferSize, ActualSendBufferSize);

	return true;
}

bool FSPSocketWorker::SocketConnect()
{
	FIPv4Address IPAddr;
	uint32 TargetIP = 0;
	FIPv4Address::Parse(IpAddress, IPAddr);
	TargetIP = IPAddr.Value;
	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr(); InternetAddr->SetIp(TargetIP);
	InternetAddr->SetPort(Port);

	const ECPSocketStatus Status = Socket->Connect(InternetAddr.Get()) ? ECPSocketStatus::Connected : ECPSocketStatus::ConnectionError; const ESocketErrors ErrorCode = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
	AsyncTask(ENamedThreads::GameThread,
		[=]()
		{
			// ChangeConnectionStatus
		});

	return Status == ECPSocketStatus::Connected;
}


bool FSPSocketWorker::SendData()
{
	while (Outbox.IsEmpty() == false)
	{
		TArray<uint8> SendItem;
		Outbox.Dequeue(SendItem);

		if (BlockingSend(SendItem.GetData(), SendItem.Num()) == false)
		{
			AsyncTask(ENamedThreads::GameThread,
				[]()
				{
					// Send data fail, stop running
				});
		}
	}

	return false;
}

void FSPSocketWorker::ReadData()
{
	TArray<uint8> ReceivedData;
	uint32 PendingDataSize = 0;
	int32 BytesReadTotal = 0;
	while (Socket->HasPendingData(PendingDataSize))
	{
		ReceivedData.SetNumUninitialized(BytesReadTotal + PendingDataSize);

		int32 BytesRead = 0;
		if (Socket->Recv(ReceivedData.GetData() + BytesReadTotal, PendingDataSize, BytesRead) == false)
		{
			ESocketErrors ErrorCode = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();

			AsyncTask(ENamedThreads::GameThread,
				[ErrorCode]()
				{
					// recevie failed
				});

			break;
		}

		BytesReadTotal += BytesRead;
	}

	if (ReceivedData.IsEmpty() == false)
	{
		Inbox.Enqueue(ReceivedData);
	}
}

void FSPSocketWorker::Sleep(const FDateTime& TickStartTime)
{
	float TickElapsedTime = (FDateTime::UtcNow() - TickStartTime).GetTotalSeconds();
	float TimeToSleep = TimeBetweenTicks - TickElapsedTime;
	if (TimeToSleep > 0.f)
	{
		FPlatformProcess::Sleep(TimeToSleep);
	}
}


TArray<uint8> FSPSocketWorker::ReadFromInbox()
{
	TArray<uint8> msg;
	Inbox.Dequeue(msg);
	return MoveTemp(msg);
}

void FSPSocketWorker::AddToOutbox(const TArray<uint8>& Message)
{
	Outbox.Enqueue(Message);
}

bool FSPSocketWorker::IsValidSocketObject()
{
	bool bValidSocket = Socket.IsValid();
	if (bValidSocket == false)
	{
		AsyncTask(ENamedThreads::GameThread,
			[=]()
			{
				//Missing socket object
			});
	}

	return bValidSocket;
}


void FSPSocketWorker::SocketShutdown()
{
	if (Socket.IsValid() == false)
	{
		return;
	}

	Socket->Close();
	Socket.Reset();

	AsyncTask(ENamedThreads::GameThread,
		[=]()
		{
			//Socket shutdown

		});
}


void FSPSocketWorker::RemoveThread()
{
	if (Thread.IsValid() == false)
	{
		return;
	}

	Thread->WaitForCompletion();
	Thread.Reset();
}

bool FSPSocketWorker::BlockingSend(const uint8* Data, int32 BytesToSend)
{
	if (BytesToSend <= 0)
	{
		return true;
	}

	int32 BytesSent = 0;

	return Socket->Send(Data, BytesToSend, BytesSent);
}



uint32 FSPSocketWorker::Run()
{
	AsyncTask(ENamedThreads::GameThread,
		[]()
		{
			//Starting thread loop

		});

	while (bRun)
	{
		FDateTime TickStartTime = FDateTime::UtcNow();
		if (bConnected == false)
		{
			if (CreateSocket() == false)
			{
				bRun = false;
				continue;
			}

			bConnected = SocketConnect();

			if (bConnected == false)
			{
				bRun = false;
				continue;
			}
		}

		if (IsValidSocketObject() == false)
		{
			bRun = false;
			continue;
		}

		if (SendData() == false)
		{
			bRun = false;
			continue;
		}

		ReadData();
		Sleep(TickStartTime);
	}

	bool IsClearBox = Outbox.IsEmpty();

	AsyncTask(ENamedThreads::GameThread,
		[IsClearBox]()
		{
			// exit loop
		});

	bConnected = false;
	SocketShutdown();

	return 0;
}

void FSPSocketWorker::Stop()
{
	bRun = false;
}

void FSPSocketWorker::Exit()
{
}