// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASTestPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GAS/AttributeSet/GASCharacterAS.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "PlayerState/GASBasePlayerState.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Libraries/GASAbilitySystemLibrary.h"
#include "PlayerState/GASWorldPlayerState.h"


/*AGASTestPlayerCharacter::AGASTestPlayerCharacter()
{
}

void AGASTestPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority()) /*¼­¹ö#1#
	{		
		BindCallbacksToDependencies();
	}
}

void AGASTestPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BindCallbacksToDependencies();
}

void AGASTestPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	static const TMap<FName, FString> DefaultMeshes = {
		{ "Hair", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_HAIR_01" },
		{ "Bangs", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_HAIR_BANGS_01" },
		{ "Teeth", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_TEETH" },
		{ "Eyes", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_EYES" },
		{ "EyesLashes", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_LASHES" },
		{ "EyesCaruncle", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_CARUNCLE" },
		{ "Neck", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_BODY_PART" },
		{ "Skirt", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_CLOTH_with_bones" },
		{ "Head", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_HEAD.SK_ROGUE_F_HEAD" }
	};
	
	for (const TPair<FName, FString>& Pair : DefaultMeshes)
	{
		const FName PartName = Pair.Key;
		const FStringAssetReference AssetPath(Pair.Value);
		
		USkeletalMeshComponent* NewPart = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), PartName);
		NewPart->SetupAttachment(GetMesh());
		NewPart->RegisterComponent();
		NewPart->SetLeaderPoseComponent(GetMesh(), true, true);

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *Pair.Value));
		if (LoadedMesh)
		{
			NewPart->SetSkeletalMesh(LoadedMesh);
		}
		
		MeshParts.Add(PartName, NewPart);

		if (NewPart->GetNumMaterials() > 0)
		{
			UMaterialInterface* BaseMaterial = NewPart->GetMaterial(0);
			if (BaseMaterial)
			{
				UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMaterial, this);
				NewPart->SetMaterial(0, MID);
				PartsMIDs.Add(PartName, MID);
			}
		}
	}
}

void AGASTestPlayerCharacter::SetCustomizeMesh(const FGFCustomizerEntry& CustomizerEntry)
{
	if (!IsLocallyControlled())
	{
		Multicast_SetCustomizationMesh(CustomizerEntry);		
	}
}

USkeletalMeshComponent* AGASTestPlayerCharacter::GetMeshPart(FName CategoryName) const
{
	if (USkeletalMeshComponent* const* Found = MeshParts.Find(CategoryName))
	{
		return *Found;
	}
	return nullptr;
}

void AGASTestPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGASTestPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		
	}	
}

void AGASTestPlayerCharacter::Multicast_SetCustomizationMesh_Implementation(const FGFCustomizerEntry& CustomizerEntry)
{
	if (CustomizerEntry.SkeletalMeshName == FName(TEXT("Hair")))
	{
		if (USkeletalMeshComponent** FoundPart = MeshParts.Find(CustomizerEntry.SkeletalMeshName))
		{			
			if (FoundPart && *FoundPart && CustomizerEntry.CustomMesh)
			{				
				USkeletalMesh* LoadedMesh = CustomizerEntry.CustomMesh;				
				(*FoundPart)->SetSkeletalMesh(LoadedMesh);

				if (CustomizerEntry.CustomMaterial)
				{
					UMaterialInterface* LoadedMaterial = CustomizerEntry.CustomMaterial.Get();
					LoadedMaterial = CustomizerEntry.CustomMaterial.LoadSynchronous();
					UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(LoadedMaterial, this);
					PartsMIDs.Add(FName(TEXT("Hair")), NewMaterial);

					UMaterialInstanceDynamic** MIDPart = PartsMIDs.Find(CustomizerEntry.SkeletalMeshName);
					
					if (CustomizerEntry.CustomMaterial.GetAssetName() == FString(TEXT("MPMI_HAIR_Wavy_23")))
					{				
						(*MIDPart)->SetVectorParameterValue(FName(TEXT("CustomHairColor")), CustomizerEntry.CustomMaterialColor);
						(*FoundPart)->SetMaterial(0, NewMaterial);
					}
					else
					{												
						(*FoundPart)->SetMaterial(0, NewMaterial);															
					}					
				}												
			}
		}
		if (USkeletalMeshComponent** FoundPart = MeshParts.Find(TEXT("Bangs")))
		{			
			if (FoundPart && *FoundPart && CustomizerEntry.SubCustomMesh)
			{				
				USkeletalMesh* LoadedMesh = CustomizerEntry.SubCustomMesh;		
				(*FoundPart)->SetSkeletalMesh(LoadedMesh);
				(*FoundPart)->SetVisibility(true);

				if (CustomizerEntry.CustomMaterial)
				{
					UMaterialInterface* LoadedMaterial = CustomizerEntry.CustomMaterial.Get();
					LoadedMaterial = CustomizerEntry.CustomMaterial.LoadSynchronous();
					UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(LoadedMaterial, this);
					PartsMIDs.Add(FName(TEXT("Bangs")), NewMaterial);

					UMaterialInstanceDynamic** MIDPart = PartsMIDs.Find(TEXT("Bangs"));

					if (CustomizerEntry.CustomMaterial.GetAssetName() == FString(TEXT("MPMI_HAIR_Wavy_23")))
					{						
						(*MIDPart)->SetVectorParameterValue(FName(TEXT("CustomHairColor")), CustomizerEntry.CustomMaterialColor);
						(*FoundPart)->SetMaterial(0, NewMaterial);
					}
					else
					{						
						(*FoundPart)->SetMaterial(0, NewMaterial);
					}	
				}							
			}
			else
			{
				(*FoundPart)->SetVisibility(false);
			}
		}
	}	
}

void AGASTestPlayerCharacter::BindCallbacksToDependencies()
{	
	if (AGASWorldPlayerState* MyPlayerState = GetPlayerState<AGASWorldPlayerState>())
	{
		if (UCustomizerComponent* CustomizerComponent = ICustomizerInterface::Execute_GetCustomizerComponent(MyPlayerState))
		{
			CustomizerComponent->CustomizerList.CustomizerChangedDelegate.AddLambda(
				[this](const FGFCustomizerEntry& CustomizerEntry)
				{
					SetCustomizeMesh(CustomizerEntry);
				});
		}
	}
}*/