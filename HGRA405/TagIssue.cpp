#include "TagIssue.h"

TagIssue::TagIssue():pointor(0),out_v(0),out_t(0) { tag_container.resize(4); tag_container_init(); }
TagIssue::~TagIssue() {}

void TagIssue::tag_container_init()
{
	for (std::vector<int>::size_type i = 0; i < tag_container.size(); i++)
	{
		tag_container[i] = i;
	}
}

void TagIssue::tag_issue()
{
	if (!out_v)
	{
		out = tag_container[pointor];
		out_v = 1;
		pointor++;
		if (pointor == 4)
		{
			pointor = 0;
		}
		
	}
}