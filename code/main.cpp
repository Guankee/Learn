#include<iostream>
#include<vector>

std::vector<int> searchRange(std::vector<int>& nums, int target) {
	bool isFind = false;
	int left = 0;
	int right = nums.size();
	int middle = -1;
	while (left < right) {
		middle = left + ((right - left) / 2);
		if (nums[middle] == target) {
			isFind = true;
			break;
		}
		else if (nums[middle] > target) {
			right = middle;
		}
		else {
			left = middle + 1;
		}
	}
	if (isFind) {
		left = middle;
		right = middle;
		while (left>=0&&nums[middle] == nums[left]) {
			left--;
		}
		while (right< nums.size()&&nums[middle] == nums[right]) {
			right++;
		}
		return std::vector<int>{left + 1, right - 1};

	}
	else {
		return std::vector<int>{-1, -1};
	}
}
int main()
{
	std::vector<int>nums{1};
	auto ret = searchRange(nums,1);
	std::cout << ret[0] << " " << ret[1] << std::endl;
	return 0;
}