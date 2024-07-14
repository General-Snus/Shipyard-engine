#include "../CommandBuffer.h"

CommandBuffer& CommandBuffer::MainEditorCommandBuffer()
{
	static CommandBuffer instance;
	return instance;
}
