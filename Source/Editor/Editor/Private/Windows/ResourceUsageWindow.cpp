#include "Windows/ResourceUsageWindow.h"
#include "EditorConfig.h"
#include "ImGuiHeader.h"
#include "RHI.h"
#include "String/Format.h"
#include "Translator.h"

namespace Bloodshot
{
	void FResourceUsageWindow::Input()
	{
	}

	void FResourceUsageWindow::Tick()
	{
	}

	void FResourceUsageWindow::Render()
	{
		if (ImGui::Ext::Begin(TRANSLATE("Resource Usage"), bFocused))
		{
			const FCurrentMemoryUsageInfo& CurrentMemoryUsageInfo = FAllocationLogger::GetCurrentMemoryUsageInfo();
			ImGui::Text("Memory Used: %f MB", (float)CurrentMemoryUsageInfo.Size / 1024.f / 1024.f);
			const FAllocationsInfo& AllocationsInfo = FAllocationLogger::GetAllocationsInfo();
			ImGui::Text("Memory Usage Volume: %f MB", (double)AllocationsInfo.AllocatedSize / 1024.0 / 1024.0);
			ImGui::Dummy(ImVec2(0.f, 8.f));

			constexpr size_t RenderResourceAllocatorCount = 7;
			const FRenderResourceAllocatorsStats& RenderResourceAllocatorsStats = FRHI::GetRenderResourceAllocatorsStats();
			const FPoolAllocatorStats* RenderResourceAllocatorsStatsPtr = (const FPoolAllocatorStats*)&RenderResourceAllocatorsStats;
			TPair<const char*, const FPoolAllocatorStats*> RenderResourceAllocatorsStatsNamed[RenderResourceAllocatorCount] =
			{
				{"Vertex Array Allocator", RenderResourceAllocatorsStatsPtr},
				{"Vertex Buffer Allocator", RenderResourceAllocatorsStatsPtr + 1},
				{"Index Buffer Allocator", RenderResourceAllocatorsStatsPtr + 2},
				{"Uniform Buffer Allocator", RenderResourceAllocatorsStatsPtr + 3},
				{"Frame Buffer Allocator", RenderResourceAllocatorsStatsPtr + 4},
				{"Shader Allocator", RenderResourceAllocatorsStatsPtr + 5},
				{"Texture Allocator", RenderResourceAllocatorsStatsPtr + 6},
			};

			ImGui::Text("Render Resource Allocators Memory");
			if (ImGui::BeginTable("Render Resource Allocators Memory", 4, ImGuiTableFlags_Borders))
			{
				ImGui::TableSetupColumn("Memory Counters", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Total Memory", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Used%", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Used", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableHeadersRow();
				for (size_t i = 0; i < RenderResourceAllocatorCount; ++i)
				{
					ImGui::TableNextColumn();
					TPair<const char*, const FPoolAllocatorStats*>& RenderResourceAllocatorStatsPair = RenderResourceAllocatorsStatsNamed[i];
					ImGui::Text(RenderResourceAllocatorStatsPair.First);
					ImGui::TableNextColumn();
					const FPoolAllocatorStats& RenderResourceAllocatorStats = *RenderResourceAllocatorStatsPair.Second;
					const size_t TotalMemory = RenderResourceAllocatorStats.ChunkSize * RenderResourceAllocatorStats.ChunkCount;
					ImGui::Text("%f MB", (float)TotalMemory / 1024.f / 1024.f);
					ImGui::TableNextColumn();
					const size_t UsedMemory = RenderResourceAllocatorStats.BusyBlockCount * RenderResourceAllocatorStats.BlockSize;
					ImGui::Text("%f%%", TotalMemory ? (float)UsedMemory / (float)TotalMemory * 100.f : 0.f);
					ImGui::TableNextColumn();
					ImGui::Text("%f MB", (float)UsedMemory / 1024.f / 1024.f);
				}
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}
}
