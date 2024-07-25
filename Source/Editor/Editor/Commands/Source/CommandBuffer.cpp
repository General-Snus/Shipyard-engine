#include "../CommandBuffer.h"


void BaseCommand::SetMergeBlocker(bool arg)
{
	m_MergeBlocker = arg;
}

CommandBuffer& CommandBuffer::MainEditorCommandBuffer()
{
	static CommandBuffer instance;
	return instance;
}
