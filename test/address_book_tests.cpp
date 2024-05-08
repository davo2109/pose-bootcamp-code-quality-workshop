#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "address_book.hpp"

TEST_CASE("entries can be added and removed") {
	address_book ab;
	CHECK_FALSE(ab.has_entry("Jane Doe"));
	ab.add_entry("Jane Doe");
	CHECK(ab.has_entry("Jane Doe"));
	ab.remove_entry("Jane Doe");
	CHECK_FALSE(ab.has_entry("Jane Doe"));
}

TEST_CASE("is name case insensitive") {
	address_book ab;
	ab.add_entry("JANE DOE");
	CHECK(ab.has_entry("jane doe"));
	ab.add_entry("daniel vogel");	
	CHECK(ab.has_entry("DANIEL VOGEL"));
}

TEST_CASE("name too short or too long") {
	address_book ab;
	CHECK_THROWS(ab.add_entry(""));
	CHECK_THROWS(ab.add_entry("very long name qwertzuiolkjhgfdsayxcvvbnhddfwersrhtrijsijdfijijfijsidjjdjjgfjhefaffsdfastwbmtsdfhrehh"));
}

TEST_CASE("entries sorted alphabetically") {
	address_book ab;
	ab.add_entry("Jane Doe");
	ab.add_entry("Daniel Vogel");
	ab.add_entry("Z Z");
	auto entries = ab.get_entries();
	CHECK(std::is_sorted(entries.begin(), entries.end()));
}

TEST_CASE("first letter capitalized") {
	address_book ab;
	ab.add_entry("daniel abc dfg HIJ Klm NOp vogel");
	auto entries = ab.get_entries();
	for(auto & entry : entries) {
		
		for(int i = 0; i < entry.size(); ++i) {
			if(i == 0 || std::isspace(entry[i-1])) {
				CHECK(std::isupper(entry[i]));
			}
		} 
	}
}

TEST_CASE("set phone or birthday on non existing") {
	address_book ab;
	ab.add_entry("Jane Doe");
	CHECK_NOTHROW(ab.set_birthday("Jane Doe", std::chrono::month_day{std::chrono::month{11}, std::chrono::day{12}}));
	CHECK_THROWS(ab.set_birthday("daniel vogel", std::chrono::month_day{std::chrono::month{9}, std::chrono::day{21}}));
	CHECK_THROWS(ab.set_birthday("Jane Doe", std::chrono::month_day{std::chrono::month{13}, std::chrono::day{32}}));
	CHECK_NOTHROW(ab.set_phone_number("Jane Doe", 123456789));
	CHECK_THROWS(ab.set_phone_number("daniel vogel", 9876543210));
}

class dummy_synchronization_provider : public synchronization_provider {
  public:
	dummy_synchronization_provider() = default;
	std::vector<std::string> synchronize(std::vector<std::string> serialized_entries) override {
		synchronization_list = merge_entries(serialized_entries, synchronization_list);
		return synchronization_list;
	}
	std::vector<std::string> synchronization_list;
};

TEST_CASE("test file synchronization") {
	address_book ab1;
	address_book ab2;
	ab1.add_entry("Jane Doe");
	ab1.add_entry("Daniel Vogel");
	dummy_synchronization_provider fsprovider;
	ab1.synchronize(fsprovider);
	ab2.synchronize(fsprovider);
	CHECK(ab2.get_entries().size() == 2);
}



