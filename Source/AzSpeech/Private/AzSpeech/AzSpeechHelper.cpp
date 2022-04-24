// Author: Lucas Vilas-Boas
// Year: 2022
// Repo: https://github.com/lucoiso/UEAzSpeech

#include "AzSpeech/AzSpeechHelper.h"
#include "Sound/SoundWave.h"
#include "Misc/FileHelper.h"

USoundWave* CreateNewSoundWave()
{
	const FString ObjectName = "Transient_AzSpeechSoundWave";
	USoundWave* SoundWave = FindObject<USoundWave>(ANY_PACKAGE, *ObjectName);

	if (!IsValid(SoundWave))
	{
		SoundWave = NewObject<USoundWave>(GetTransientPackage(), *ObjectName, RF_Transient);
	}
	else
	{
		if (SoundWave->RawData.IsLocked())
		{
			SoundWave->RawData.Unlock();
		}

		SoundWave->RawData.RemoveBulkData();
	}

	return SoundWave;
}

USoundWave* UAzSpeechHelper::ConvertFileIntoSoundWave(const FString FilePath, const FString FileName)
{
	if (!FilePath.IsEmpty() && !FileName.IsEmpty())
	{
		const auto QualifiedFileInfo = [FilePath, FileName]() -> const FString
		{
			FString LocalPath = FilePath;
			if (*FilePath.end() != '\\')
			{
				LocalPath += '\\';
			}

			FString LocalName = FileName;
			if (FileName.Right(FileName.Len() - 4) != ".wav")
			{
				LocalName += ".wav";
			}

			return LocalPath + LocalName;
		};

		if (TArray<uint8> RawData; FFileHelper::LoadFileToArray(RawData, *QualifiedFileInfo(), FILEREAD_NoFail))
		{
			return ConvertStreamIntoSoundWave(RawData);
		}
	}

	return nullptr;
}

USoundWave* UAzSpeechHelper::ConvertStreamIntoSoundWave(TArray<uint8> RawData)
{
#if ENGINE_MAJOR_VERSION >= 5
	if (!RawData.IsEmpty())
#else	
	if (RawData.Num() != 0)
#endif
	{
		USoundWave* SoundWave = CreateNewSoundWave();

		SoundWave->RawData.Lock(LOCK_READ_WRITE);
		void* RawDataPtr = SoundWave->RawData.Realloc(RawData.Num());
		FMemory::Memcpy(RawDataPtr, RawData.GetData(), RawData.Num());
		SoundWave->RawData.Unlock();

		FWaveModInfo WaveInfo;
		WaveInfo.ReadWaveInfo(RawData.GetData(), RawData.Num());

		const int32 ChannelCount = *WaveInfo.pChannels;
		const int32 SizeOfSample = *WaveInfo.pBitsPerSample / 8;
		const int32 NumSamples = WaveInfo.SampleDataSize / SizeOfSample;
		const int32 NumFrames = NumSamples / ChannelCount;

		SoundWave->Duration = NumFrames / *WaveInfo.pSamplesPerSec;
		SoundWave->SetSampleRate(*WaveInfo.pSamplesPerSec);
		SoundWave->NumChannels = ChannelCount;
		SoundWave->TotalSamples = *WaveInfo.pSamplesPerSec * SoundWave->Duration;
		
#if ENGINE_MAJOR_VERSION >= 5
		SoundWave->SetImportedSampleRate(*WaveInfo.pSamplesPerSec);
#endif

		SoundWave->InitAudioResource(SoundWave->RawData);

		return SoundWave;
	}

	return nullptr;
}
