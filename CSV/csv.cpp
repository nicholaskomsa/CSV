#include <fstream>
#include <ranges>
#include <string>
#include <vector>
#include <print>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;
using namespace views;

template<typename... Values>
void splitLine(string_view line, Values&&... values){
	
	auto lineElements = line | split(',');
	auto begin = lineElements.begin();

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
	milliseconds mTimestamp;
	float mA, mB;

	CSV(string_view line) {
		fromString(line);
	}

	void fromString(string_view line) {

		uint64_t timestamp;
		splitLine(line, timestamp, mA, mB);
		mTimestamp = milliseconds(timestamp);
	}
	string toString() const {
		return format("{},{},{}", mTimestamp, mA, mB);
	}
};

vector<CSV> loadAndParseCSV(const string& path = "C:/Users/nicho/OneDrive/Desktop/piezo_id9.csv") {

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

	for (const auto& i : iota(1ULL, data.size()) | views::reverse)
		data[i].mTimestamp -= data[i - 1].mTimestamp;
	data[0].mTimestamp = milliseconds(0);
	};

int main() {

	auto data = loadAndParseCSV();

	elapsedTimeTransform(data);

	for (auto& entry : data)
		println("{}", entry.toString());

	return 0;
}
