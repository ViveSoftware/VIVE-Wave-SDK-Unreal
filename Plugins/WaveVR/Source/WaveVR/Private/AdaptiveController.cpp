// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "AdaptiveController.h"
#include "WaveVRPrivatePCH.h"
#include "AdaptiveControllerLoader.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY(LogAssimp);
DEFINE_LOG_CATEGORY_STATIC(AdaptiveController, Display, All);

AAdaptiveController::AAdaptiveController()
{
	LOGD(LogAssimp, "AAdaptiveController constructor");
	if (UAdaptiveControllerLoader::GetRenderModelPath().Contains(FString(TEXT("Unreal")), ESearchCase::CaseSensitive, ESearchDir::FromEnd)) {
		bUsingNewFbxDesigned = false;
	}
	//PrimaryComponentTick.bCanEverTick = true; /*To improve performance.*/ /*TODO: Turn on if needed*/
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ControllerModelSceneRoot"));
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	if (bUsingNewFbxDesigned) {
		SceneComponent->SetRelativeRotation(FRotator(180, -90, 90));
	} else {
		SceneComponent->SetRelativeRotation(FRotator(0, 0, -90));
	}
	SceneComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	if (UAdaptiveControllerLoader::GetSpawnAdaptiveControllerFlag()) {
		LOGD(LogAssimp, "getSpawnAdaptiveControllerFlag is true!! start to load FBX");
		//Load FBX file
		FString FilePath = "";
		if (bUsingNewFbxDesigned) {
			FilePath = UAdaptiveControllerLoader::GetRenderModelPath() + "controller00.fbx";
		} else {
			FilePath = UAdaptiveControllerLoader::GetRenderModelPath() + "Unreal.fbx";
		}
		LOGD(AdaptiveController, "FBX file: %s", TCHAR_TO_ANSI(*FilePath));
		int32 SectionCount;
		FString ErrorCode;
		bool ret = OpenMesh(FilePath, SectionCount, ErrorCode);
		LOGD(AdaptiveController, "AAdaptiveController constructor(%d %s)", SectionCount, TCHAR_TO_ANSI(*ErrorCode));
		if (ret) {
			//Create UProceduralMeshComponent for multiple mesh.
			for (int32 i = 0; i < Multi_meshCurrentlyProcessed; i++) {
				LOGD(AdaptiveController, "NewObject<UProceduralMeshComponent>(%s)", TCHAR_TO_ANSI(*Multi_meshNames[i]));
				UProceduralMeshComponent* CurrMesh = NewObject<UProceduralMeshComponent>(this, FName(*Multi_meshNames[i]));
				CurrMesh->bUseAsyncCooking = true; // New in UE 4.17, multi-threaded PhysX cooking.
				if (FName(*Multi_meshNames[i]) == FName(TEXT("__CM__Emitter"))) {
					LOGD(AdaptiveController, "Emitter is found");
					CurrMesh->SetupAttachment(RootComponent);
					CurrMesh->SetRelativeTransform(FTransform(Multi_TransformsOfMeshes[i]));
					CurrMesh->SetVisibility(true);
				}
				else if (FName(*Multi_meshNames[i]) == FName(TEXT("__CM__Battery"))) {
					LOGD(AdaptiveController, "Battery is found");
					CurrMesh->SetupAttachment(SceneComponent);
					CurrMesh->SetRelativeTransform(FTransform(Multi_TransformsOfMeshes[i]));
					CurrMesh->SetVisibility(false);
				} else {
					CurrMesh->SetRelativeTransform(FTransform(Multi_TransformsOfMeshes[i]));
					CurrMesh->SetupAttachment(SceneComponent);
					CurrMesh->SetVisibility(bUsingNewFbxDesigned ? false : true); //Hide to decrease draw call.
				}
				Multi_meshes.Add(CurrMesh);
			}

			//Create UProceduralMeshComponent for one (merged) mesh.
			if (bUsingNewFbxDesigned) {
				for (int32 i = 0; i < One_meshCurrentlyProcessed; i++) {
					bool bMatch = false;
					for (int32 j = 0; j < Multi_meshes.Num(); j++) {
						if (Multi_meshNames[j] == One_meshNames[i]){
							bMatch = true;
							LOGD(AdaptiveController, "Set Multi_meshes (%s) Visible as true", TCHAR_TO_ANSI(*Multi_meshNames[j]));
							Multi_meshes[j]->SetVisibility(true);
							break;
						}
					}
					if (!bMatch) {
						LOGD(AdaptiveController, "NewObject<UProceduralMeshComponent>(%s)", TCHAR_TO_ANSI(*One_meshNames[i]));
						FString meshname = FString(*One_meshNames[i]);
						FString fs = "Merge_" + meshname;
						FName fn = FName(*fs);
						UProceduralMeshComponent* CurrMesh = NewObject<UProceduralMeshComponent>(this, fn);
						CurrMesh->bUseAsyncCooking = true; // New in UE 4.17, multi-threaded PhysX cooking.
						CurrMesh->SetRelativeTransform(FTransform(One_TransformsOfMeshes[i]));
						CurrMesh->SetupAttachment(SceneComponent);
						CurrMesh->SetVisibility(true);
						One_meshes.Add(CurrMesh);
						KeepIndex.Add(i);
					}
				}
			}

			UAdaptiveControllerLoader::SetSpawnActorResult(true);
		}
		else {
			UAdaptiveControllerLoader::SetSpawnActorResult(false);
		}
		UAdaptiveControllerLoader::SetUseNewModel(bUsingNewFbxDesigned);
	}
	else {
		LOGD(LogAssimp, "getSpawnAdaptiveControllerFlag is false!!");
		UAdaptiveControllerLoader::SetSpawnActorResult(false);
		UAdaptiveControllerLoader::SetUseNewModel(false);
	}
	LOGD(LogAssimp, "AAdaptiveController constructor--");
}

// Called when the game starts or when spawned
void AAdaptiveController::BeginPlay()
{
	Super::BeginPlay();
}

void AAdaptiveController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// This is called when actor is spawned
void AAdaptiveController::PostActorCreated()
{
	CreateProceduralMeshComponent();
	Super::PostActorCreated();
}

void AAdaptiveController::CreateProceduralMeshComponent()
{
	LOGD(LogAssimp, "AAdaptiveController CreateProceduralMeshComponent");
#if WAVEVR_SUPPORTED_PLATFORMS
	for (int32 i = 0; i < Multi_meshCurrentlyProcessed; i++) {
		Multi_meshes[i]->CreateMeshSection_LinearColor(0, Multi_vertices[i], Multi_indices[i], Multi_normals[i], Multi_uvs[i], Multi_vertexColors[i], Multi_tangents[i], /*true*/false);
		// Enable collision data
		Multi_meshes[i]->ContainsPhysicsTriMeshData(/*true*/false);
	}
	if (bUsingNewFbxDesigned) {
		for (int32 i = 0; i < One_meshes.Num(); i++) {
			LOGD(AdaptiveController, "CreateMeshSection_LinearColor (%s) Create Mesh", TCHAR_TO_ANSI(*One_meshNames[i]));
			One_meshes[i]->CreateMeshSection_LinearColor(0, One_vertices[KeepIndex[i]], One_indices[KeepIndex[i]], One_normals[KeepIndex[i]], One_uvs[KeepIndex[i]], One_vertexColors[KeepIndex[i]], One_tangents[KeepIndex[i]], /*true*/false);
			One_meshes[i]->ContainsPhysicsTriMeshData(/*true*/false);
		}
	}
#endif
	LOGD(LogAssimp, "AAdaptiveController CreateProceduralMeshComponent--");
}

void AAdaptiveController::PostLoad()
{
	Super::PostLoad();
	CreateProceduralMeshComponent();
}

bool AAdaptiveController::OpenMesh(FString path, int32& SectionCount, FString& ErrorCode)
{
	Clear();

	std::string filename(TCHAR_TO_UTF8(*path));
#if WAVEVR_SUPPORTED_PLATFORMS

	TArray<const aiScene*> Scenes;

	//Multi
	Assimp::Importer importerMulti;
	const aiScene* MultiMeshScene = importerMulti.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	Scenes.Add(MultiMeshScene);

	//One
	Assimp::Importer importerOne;
	const aiScene* OneMeshScene = nullptr;
	if(bUsingNewFbxDesigned) {
		OneMeshScene = importerOne.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded | /*aiProcess_PreTransformVertices*/aiProcess_OptimizeGraph);
		Scenes.Add(OneMeshScene);
	}

	for (int32 i = 0; i< Scenes.Num(); i++) {
		const aiScene* scene = Scenes[i];
		if (!scene) {
			if (scene == MultiMeshScene) {
				ErrorCode = importerMulti.GetErrorString();
			} else if (OneMeshScene != nullptr && scene == OneMeshScene) {
				ErrorCode = importerOne.GetErrorString();
			}
			LOGD(LogAssimp, "Assimp Encounter Error: Can not get aiScene!");
			return false;
		}

		if (scene == MultiMeshScene) {
			LOGD(LogAssimp, "[Multi Mesh] Open file success!");
		} else if (scene == OneMeshScene) {
			LOGD(LogAssimp, "[One Mesh] Open file Success!");
		}

		ClearProcessedMeshData();

		aiMatrix4x4 transformInit;
		//aiMatrix4x4t<float>::RotationZ(FMath::DegreesToRadians(90), transformInit);
		processNode(scene->mRootNode, scene, transformInit);
		SectionCount = _meshCurrentlyProcessed;

#if 0 //debug
		LOGI(LogAssimp, "_meshCurrentlyProcessed(%d)", _meshCurrentlyProcessed);
		for (int32 i = 0; i < _meshCurrentlyProcessed; i++) {
			LOGI(Assimp, "SectionNumber(%d)", i);
			for (FVector element : _vertices[i])
			{
				LOGI(LogAssimp, "_vertices(%f,%f,%f)", element.X,element.Y,element.Z);
			}
			for (int32 element : _indices[i])
			{
				LOGI(LogAssimp, "_indices(%d)", element);
			}
			for (FVector element : _normals[i])
			{
				LOGI(LogAssimp, "_normals(%f,%f,%f)", element.X,element.Y,element.Z);
			}
			for (FVector2D element : _uvs[i])
			{
				LOGI(LogAssimp, "_uvs(%f,%f)", element.X,element.Y);
			}
			for (FProcMeshTangent element : _tangents[i])
			{
				LOGI(LogAssimp, "_tangents(%f,%f,%f)", element.X,element.Y,element.Z);
			}
		}
#endif
		if (scene == MultiMeshScene) {
			CopyProcessedRawDataToMultiMesh();
		} else if (OneMeshScene != nullptr && scene == OneMeshScene) {
			CopyProcessedRawDataToOneMesh();
		}
	}
#endif
	return true;
}

bool AAdaptiveController::GetSection(int32 index, TArray<FVector>& Vertices, TArray<int32>& Faces, TArray<FVector>& Normals, TArray<FVector2D>& UV, TArray<FProcMeshTangent>& Tangents)
{
	if (index >= Multi_meshCurrentlyProcessed)
	{
		return false;
	}
	Vertices = Multi_vertices[index];
	Faces = Multi_indices[index];
	Normals = Multi_normals[index];
	UV = Multi_uvs[index];
	Tangents = Multi_tangents[index];
	return true;
}

void AAdaptiveController::Clear()
{
	Multi_meshNames.Empty();
	Multi_vertices.Empty();
	Multi_indices.Empty();
	Multi_normals.Empty();
	Multi_uvs.Empty();
	Multi_tangents.Empty();
	Multi_vertexColors.Empty();
	Multi_meshCurrentlyProcessed = 0;
	Multi_TransformsOfMeshes.Empty();

	One_meshNames.Empty();
	One_vertices.Empty();
	One_indices.Empty();
	One_normals.Empty();
	One_uvs.Empty();
	One_tangents.Empty();
	One_vertexColors.Empty();
	One_meshCurrentlyProcessed = 0;
	One_TransformsOfMeshes.Empty();
	KeepIndex.Empty();
}

void AAdaptiveController::NodeInfo(aiNode* node) {
	LOGI(LogAssimp, "aiNode mName(%s), mNumChildren(%u), mNumMeshes(%u)",
			node->mName.C_Str(), node->mNumChildren, node->mNumMeshes);
	LOGI(LogAssimp, "Node->mTransformation is\n  (%lf,%lf,%lf,%lf)\n  (%lf,%lf,%lf,%lf)\n  (%lf,%lf,%lf,%lf)\n  (%lf,%lf,%lf,%lf)\n",
		node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
		node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
		node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
		node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4);
}

void AAdaptiveController::MeshInfo(aiMesh* aimesh) {
	LOGI(LogAssimp, "aimesh mMaterialIndex(%d), mNumAnimMeshes(%d), mMethod(%d)",
			aimesh->mMaterialIndex, aimesh->mNumAnimMeshes, aimesh->mMethod);

	if(aimesh->HasBones()) {
		LOGD(LogAssimp, "aimesh have bones");
		aiBone** Bones= aimesh->mBones;
		for (uint32 j = 0; j < aimesh->mNumBones ; j++) {
			aiBone* Bone_Ele = Bones[j];
			LOGI(LogAssimp, "Bone element Name(%s)", Bone_Ele->mName.C_Str());
			aiMatrix4x4 OffsetMat = Bone_Ele->mOffsetMatrix;
			for (uint32 m = 0; m < 16 ; m++) {
				LOGI(LogAssimp, "Bone Matrix[%u]element Name(%f)", m, *OffsetMat[m]);
			}
			LOGI(LogAssimp, "Bone mNumWeights[%u]", Bone_Ele->mNumWeights);
		}
	} else {
		LOGD(LogAssimp, "aimesh do NOT have bones");
	}
}

void AAdaptiveController::processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 AccuTransform)
{
	aiMatrix4x4 transform = node->mTransformation * AccuTransform;
	//NodeInfo(node);
	for (uint32 i = 0; i < node->mNumMeshes; i++) {
		aiMatrix4x4 transformMesh = transform;
		aiMesh* AImesh = scene->mMeshes[node->mMeshes[i]];
	FString MeshName = AImesh->mName.C_Str();
		//_meshNames[_meshCurrentlyProcessed] = FString(AImesh->mName.C_Str());
		//_meshNames.Add(TEXT(AImesh->mName.C_Str()));
		_meshNames.Add(MeshName);

		//Apply scale, unit of c4d is mm and Assimp is cm.
		double unitSize;
		scene->mMetaData->Get("UnitScaleFactor", unitSize);
		transformMesh.a4 *= unitSize;
		transformMesh.b4 *= unitSize;
		transformMesh.c4 *= unitSize;
		LOGI(LogAssimp, "UnitScaleFactor is (%lf)" , unitSize);

#if !UE_BUILD_SHIPPING //Do not log for performance purpose.
		MeshInfo(AImesh);
		LOGI(LogAssimp, "_meshCurrentlyProcessed(%d)\n AImesh->mName(%s)" , _meshCurrentlyProcessed, AImesh->mName.C_Str());
		LOGI(LogAssimp, "Mesh transformMesh is\n  (%lf,%lf,%lf,%lf)\n  (%lf,%lf,%lf,%lf)\n  (%lf,%lf,%lf,%lf)\n  (%lf,%lf,%lf,%lf)\n",
			transformMesh.a1, transformMesh.a2, transformMesh.a3, transformMesh.a4,
			transformMesh.b1, transformMesh.b2, transformMesh.b3, transformMesh.b4,
			transformMesh.c1, transformMesh.c2, transformMesh.c3, transformMesh.c4,
			transformMesh.d1, transformMesh.d2, transformMesh.d3, transformMesh.d4);
#endif

		FMatrix NodeTransform(
			FPlane(transformMesh.a1, transformMesh.b1, transformMesh.c1, transformMesh.d1),
			FPlane(transformMesh.a2, transformMesh.b2, transformMesh.c2, transformMesh.d2),
			FPlane(transformMesh.a3, transformMesh.b3, transformMesh.c3, transformMesh.d3),
			FPlane(transformMesh.a4, transformMesh.b4, transformMesh.c4, transformMesh.d4));
		_TransformsOfMeshes.Add(NodeTransform);
		LOGD(AdaptiveController, "_meshCurrentlyProcessed(%s) FString", TCHAR_TO_ANSI(*_meshNames[_meshCurrentlyProcessed]));
		processMesh(AImesh, scene);
		++_meshCurrentlyProcessed;
	}

	//uint32 nodes = node->mNumMeshes;
	// do the same for all of its children
	for (uint32 i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, transform);
	}
}

void AAdaptiveController::processMesh(aiMesh* meshData, const aiScene* scene)
{
	bool _requiresFullRecreation = false;
	//Init container
	if (_vertices.Num() <= _meshCurrentlyProcessed) {
		_vertices.AddZeroed();
		_normals.AddZeroed();
		_uvs.AddZeroed();
		_tangents.AddZeroed();
		_vertexColors.AddZeroed();
		_indices.AddZeroed();
	}

	// we check whether the current data to read has a different amount of vertices compared to the last time we generated the mesh
	// if so, it means we'll need to recreate the mesh and resupply new indices.
	if (meshData->mNumVertices != _vertices[_meshCurrentlyProcessed].Num())
		_requiresFullRecreation = true;

	// we reinitialize the arrays for the new data we're reading
	_vertices[_meshCurrentlyProcessed].Empty();
	_normals[_meshCurrentlyProcessed].Empty();
	_uvs[_meshCurrentlyProcessed].Empty();
	// this if actually seems useless, seeing what it does without it
	//if (_requiresFullRecreation) {
	_tangents[_meshCurrentlyProcessed].Empty();
	_vertexColors[_meshCurrentlyProcessed].Empty();
	_indices[_meshCurrentlyProcessed].Empty();
	//}

	//Apply scale, unit of c4d is mm and Assimp is cm.
	double unitSize;
	scene->mMetaData->Get("UnitScaleFactor", unitSize);

	for (unsigned int i = 0; i < meshData->mNumVertices; i++) {
		FVector vertex, normal;
		// process vertex positions, normals and UVs
		vertex.X = meshData->mVertices[i].x * unitSize;
		vertex.Y = meshData->mVertices[i].y * unitSize;
		vertex.Z = meshData->mVertices[i].z * unitSize;

		normal.X = meshData->mNormals[i].x * unitSize;
		normal.Y = meshData->mNormals[i].y * unitSize;
		normal.Z = meshData->mNormals[i].z * unitSize;

		// if the mesh contains tex coords
		if (meshData->mTextureCoords[0]) {
			FVector2D uvs;
			uvs.X = meshData->mTextureCoords[0][i].x;
			uvs.Y = meshData->mTextureCoords[0][i].y;
			_uvs[_meshCurrentlyProcessed].Add(uvs);
		}
		else {
			_uvs[_meshCurrentlyProcessed].Add(FVector2D(0.f, 0.f));
		}
		_vertices[_meshCurrentlyProcessed].Add(vertex);
		_normals[_meshCurrentlyProcessed].Add(normal);
		_tangents[_meshCurrentlyProcessed].Add(FProcMeshTangent(0, 1, 0));
	}

	if (_requiresFullRecreation) {
		// process indices
		for (uint32 i = 0; i < meshData->mNumFaces; i++) {
			aiFace face = meshData->mFaces[i];
			_indices[_meshCurrentlyProcessed].Add(face.mIndices[2]);
			_indices[_meshCurrentlyProcessed].Add(face.mIndices[1]);
			_indices[_meshCurrentlyProcessed].Add(face.mIndices[0]);
		}
	}
}

void AAdaptiveController::ClearProcessedMeshData()
{
	_meshNames.Empty();
	_vertices.Empty();
	_indices.Empty();
	_normals.Empty();
	_uvs.Empty();
	_tangents.Empty();
	_vertexColors.Empty();
	_meshCurrentlyProcessed = 0;
	_TransformsOfMeshes.Empty();
}

void AAdaptiveController::CopyProcessedRawDataToMultiMesh()
{
	Multi_meshNames    = _meshNames;
	Multi_vertices     = _vertices;
	Multi_indices      = _indices;
	Multi_normals      = _normals;
	Multi_uvs          = _uvs;
	Multi_tangents     = _tangents;
	Multi_vertexColors = _vertexColors;
	Multi_meshCurrentlyProcessed = _meshCurrentlyProcessed;
	Multi_TransformsOfMeshes = _TransformsOfMeshes;
}

void AAdaptiveController::CopyProcessedRawDataToOneMesh()
{
	One_meshNames = _meshNames;
	One_vertices = _vertices;
	One_indices  = _indices;
	One_normals  = _normals;
	One_uvs      = _uvs;
	One_tangents = _tangents;
	One_vertexColors = _vertexColors;
	One_meshCurrentlyProcessed = _meshCurrentlyProcessed;
	One_TransformsOfMeshes = _TransformsOfMeshes;
}
