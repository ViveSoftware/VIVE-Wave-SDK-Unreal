// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AdaptiveController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAssimp, Log, All);

UCLASS()
class WAVEVR_API AAdaptiveController : public AActor
{
	GENERATED_BODY()
//Life Cycle
public:
	// Sets default values for this actor's properties
	AAdaptiveController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// This is called when actor is spawned (at runtime or when you drop it into the world in editor)
	virtual void PostActorCreated() override;
	// This is called when actor is already in level and map is opened
	virtual void PostLoad() override;

private:
  //  UFUNCTION(BlueprintCallable, Category = "WaveVR|Assimp")
	bool OpenMesh(FString path, int32& SectionCount, FString& ErrorCode);

  //  UFUNCTION(BlueprintCallable, Category = "WaveVR|Assimp")
	bool GetSection(int32 index, TArray<FVector>& Vertices, TArray<int32>& Faces, TArray<FVector>& Normals, TArray<FVector2D>& UV, TArray<FProcMeshTangent>& Tangents);

  //  UFUNCTION(BlueprintCallable, Category = "WaveVR|Assimp")
	void Clear();

private:

	void ClearProcessedMeshData();
	void CopyProcessedRawDataToMultiMesh();
	void CopyProcessedRawDataToOneMesh();

	void processMesh(aiMesh* mesh, const aiScene* scene);
	void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 AccuTransform);
	void CreateProceduralMeshComponent();
	void MeshInfo(aiMesh* aimesh);
	void NodeInfo(aiNode* node);

	//Current Processed Mesh Data
	TArray<FString> _meshNames;
	TArray<TArray<FVector>> _vertices;
	TArray<TArray<int32>> _indices;
	TArray<TArray<FVector>> _normals;
	TArray<TArray<FVector2D>> _uvs;
	TArray<TArray<FProcMeshTangent>> _tangents;
	TArray<TArray<FLinearColor>> _vertexColors;
	int32 _meshCurrentlyProcessed;
	TArray<FMatrix> _TransformsOfMeshes;

	//Mutiple mesh Model
	UPROPERTY(VisibleAnywhere)
	TArray<UProceduralMeshComponent*> Multi_meshes;
	TArray<FString> Multi_meshNames;
	TArray<TArray<FVector>> Multi_vertices;
	TArray<TArray<int32>> Multi_indices;
	TArray<TArray<FVector>> Multi_normals;
	TArray<TArray<FVector2D>> Multi_uvs;
	TArray<TArray<FProcMeshTangent>> Multi_tangents;
	TArray<TArray<FLinearColor>> Multi_vertexColors;
	int32 Multi_meshCurrentlyProcessed;
	TArray<FMatrix> Multi_TransformsOfMeshes;

	//One mesh Model
	TArray<UProceduralMeshComponent*> One_meshes;
	TArray<FString> One_meshNames;
	TArray<TArray<FVector>> One_vertices;
	TArray<TArray<int32>> One_indices;
	TArray<TArray<FVector>> One_normals;
	TArray<TArray<FVector2D>> One_uvs;
	TArray<TArray<FProcMeshTangent>> One_tangents;
	TArray<TArray<FLinearColor>> One_vertexColors;
	int32 One_meshCurrentlyProcessed;
	TArray<FMatrix> One_TransformsOfMeshes;
	TArray<int32> KeepIndex;

	USceneComponent* SceneComponent;
	USceneComponent* EmitterScene;
	bool bUsingNewFbxDesigned = true;
};

