#include <iostream>
#include <fstream>
#include <ranges>
#include <string>
#include <vector>
#include <print>
#include <algorithm>
#include <chrono>

//to read text not binary data, a format, CSV.
//Comma Separated Values File line element
struct CSV {
	std::chrono::milliseconds mTimestamp;
	float mA, mB;

	void splitLine(std::string_view line) {

		using namespace std;

		auto charsAdvance = [&](auto& begin, auto&&... value) {

			auto parse = [&](auto& value, auto& begin) {
				string_view sv(*begin);
				from_chars(sv.data(), sv.data() + sv.size(), value);
				advance(begin, 1);
				};

			(parse(value, begin), ...);

			};

		auto values = line | std::views::split(',');

		auto begin = values.begin();
		uint64_t timestamp;
		charsAdvance(begin, timestamp, mA, mB);
		mTimestamp = std::chrono::milliseconds(timestamp);
	}

	std::string toString() const {
		return std::format("{},{},{}", mTimestamp, mA, mB);
	}
};

std::vector<CSV> loadAndParseCSV(const std::string& path = "C:/Users/nicho/OneDrive/Desktop/piezo_id9.csv") {

	using namespace std;

	ifstream fin(path);

	if (fin.fail())
		throw runtime_error(format("Failed to open file: {}", path));

	vector<CSV> data;
	data.reserve(50'000);
	string line;

	while (getline(fin, line)) {
		data.push_back({});
		data.back().splitLine(line);
	}

	println("Loaded {} CSV entries", data.size());

	return data;
}

auto elapsedTimeTransform = [](auto& data) {

	for (const auto& i : std::views::iota(1ULL, data.size()) | std::views::reverse)
		data[i].mTimestamp -= data[i - 1].mTimestamp;
	data[0].mTimestamp = std::chrono::milliseconds(0);
	};

int main() {

	auto data = loadAndParseCSV();

	elapsedTimeTransform(data);

	for (auto& entry : data)
		println("{}", entry.toString());

	return 0;
}
