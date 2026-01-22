#include <fstream>
#include <ranges>
#include <string>
#include <vector>
#include <print>
#include <algorithm>
#include <chrono>

template<typename... Values>
void charsAdvance(std::string_view line, Values&&... values){
	
	using namespace std;

	auto splitLine = line | views::split(',');
	auto begin = splitLine.begin();

	auto parse = [&](auto& value) {
		string_view sv(*begin);
		from_chars(sv.data(), sv.data() + sv.size(), value);
		advance(begin, 1);
		};

	(parse(values), ...);
}

//to read text not binary data, a format, CSV
//Comma Separated Values line to be split or joined using ','
struct CSV {
	std::chrono::milliseconds mTimestamp;
	float mA, mB;

	CSV(std::string_view line) {
		fromString(line);
	}

	void fromString(std::string_view line) {

		uint64_t timestamp;
		charsAdvance(line, timestamp, mA, mB);
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

	vector<CSV> data; data.reserve(50'000);
	string line;

	while(getline(fin, line))
		data.push_back({ line });
	
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
