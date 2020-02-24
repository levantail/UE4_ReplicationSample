// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class testQ_farm51_KondrEditorTarget : TargetRules
{
	public testQ_farm51_KondrEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("testQ_farm51_Kondr");
	}
}
