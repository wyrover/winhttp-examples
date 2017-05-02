#include <iostream>
#include "gtest/gtest.h"
#include "http_stl.h"

using namespace std;


TEST(fun, request_t_get)
{
	using namespace http::stl;

	session_t sess_("My User Agent");
	connection_t conn_(sess_, "http://www.microsoft.com/blah.html");
	request_t req("GET", "http://www.microsoft.com");
	response_t resp = conn_.send(req);

	EXPECT_EQ(200, resp.status());

	stringstream ss;
	resp.read(ss);
	EXPECT_TRUE(ss.str().length()> 0);
}

TEST(fun, request_t_header)
{
	using namespace http::stl;

	session_t sess_("My User Agent");
	connection_t conn_(sess_, "http://www.microsoft.com/blah.html");

	request_t req("GET", "/");
	req.add_header("Accept-Language: en-US");
	response_t resp = conn_.send(req);

	EXPECT_EQ(200, resp.status());

	stringstream ss;
	resp.read(ss);
	EXPECT_TRUE(ss.str().length()> 0);


}

TEST(fun, request_ssl)
{
	using namespace http::stl;

	session_t sess_("My User Agent");
	connection_t conn_(sess_, "https://www.baidu.com/");

	request_t req("GET", "/");
	response_t resp = conn_.send(req);

	EXPECT_EQ(200, resp.status());

	stringstream ss;
	resp.read(ss);
	EXPECT_TRUE(ss.str().length()> 0);


}



int main(int argc, wchar_t* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

