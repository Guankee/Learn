#include<iostream>
#include<vector>

//std::vector<int> searchRange(std::vector<int>& nums, int target) {
//	bool isFind = false;
//	int left = 0;
//	int right = nums.size();
//	int middle = -1;
//	while (left < right) {
//		middle = left + ((right - left) / 2);
//		if (nums[middle] == target) {
//			isFind = true;
//			break;
//		}
//		else if (nums[middle] > target) {
//			right = middle;
//		}
//		else {
//			left = middle + 1;
//		}
//	}
//	if (isFind) {
//		left = middle;
//		right = middle;
//		while (left>=0&&nums[middle] == nums[left]) {
//			left--;
//		}
//		while (right< nums.size()&&nums[middle] == nums[right]) {
//			right++;
//		}
//		return std::vector<int>{left + 1, right - 1};
//
//	}
//	else {
//		return std::vector<int>{-1, -1};
//	}
//}


int removeElement(std::vector<int>& nums, int val)
{
	int detNum = 0;
	for (size_t i = 0; i < nums.size(); i++)
	{
		if (val == nums[i])
		{
			detNum++;
			for (size_t j = i; j < nums.size() - detNum; j++)
			{
				nums[j] = nums[j + 1];
			}
			nums[nums.size() - detNum] = val - 1;
			i--;
		}
		
	}
	return nums.size() - detNum;
}


int main()
{
	std::vector<int>nums{ 0,1,2,2,3,0,4,2 };
	int x=removeElement(nums, 2);
	return 0;
}