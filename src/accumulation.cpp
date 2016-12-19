#include<functional>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<tuple>
#include<iostream>
#include<cstdio>
#include<type_traits>
#include<iterator>
#include<cmath>

#include<boost/lexical_cast.hpp>
#include<boost/optional.hpp>
#include<boost/filesystem/path.hpp>
#include<boost/filesystem/fstream.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/accumulators/accumulators.hpp>
#include<boost/accumulators/statistics.hpp>
#include<boost/range/algorithm/for_each.hpp>

using namespace std::string_literals;

struct tuple_displayer final{
	static void disp_impl(){}
	template<class Head,class... Tail>
	static void disp_impl(Head&& head,Tail&&... tail)
	{
		std::cout<<std::forward<Head>(head)<<" ";
		disp_impl(std::forward<Tail>(tail)...);
	}
	template<class Tuple,std::size_t... I>
	static void disp_impl(Tuple&& t,std::integer_sequence<std::size_t,I...>)
	{
		disp_impl(std::get<I>(t)...);
	}
	template<class Tuple>
	static void disp(Tuple&& t)
	{
		disp_impl(std::forward<Tuple>(t),std::make_integer_sequence<std::size_t,std::tuple_size<std::remove_reference_t<Tuple>>::value>());
	}
};

struct csv_consider final{
	typedef std::tuple<
				std::string,
				std::string,
				std::size_t,
				std::string,
				std::string,
				std::string,
				int
			>
	csv_tuple;
	typedef std::vector<csv_tuple> csv_vector;
	
	bool file_exists;

	csv_consider(const boost::filesystem::path& root)noexcept
		:file_exists(!boost::filesystem::exists(root)?false:true)
	{
		using namespace std::string_literals;
		if(file_exists){
			for(boost::filesystem::directory_iterator iter(root),end; iter!=end; ++iter){
				if(iter->path().extension()==".csv"s && !boost::filesystem::is_directory(iter->path())){
					std::ifstream ifs("csv/"s+iter->path().filename().generic_string());
					if(!ifs){
						assert("failed to load csv");
						break;
					}

					std::string str;
					typename std::tuple_element<0,csv_tuple>::type date;
					typename std::tuple_element<1,csv_tuple>::type place;
					typename std::tuple_element<2,csv_tuple>::type number=0;
					typename std::tuple_element<3,csv_tuple>::type home_team;
					typename std::tuple_element<4,csv_tuple>::type score;
					typename std::tuple_element<5,csv_tuple>::type away_team;
					typename std::tuple_element<6,csv_tuple>::type toto_result=0;
					
					while(std::getline(ifs,str)){
						std::string token;
						std::istringstream stream(str);
						for(std::size_t i=0; std::getline(stream,token,','); ++i){
							switch(i){
								case 0: date=csv_consider::str_deleter(token," "); break;
								case 1: place=csv_consider::str_deleter(token," "); break;
								case 2: std::sscanf(token.c_str(),"%ld",&number); break;
								case 3: home_team=csv_consider::str_deleter(token," "); break;
								case 4: score=csv_consider::str_deleter(token," "); break;
								case 5: away_team=csv_consider::str_deleter(token," "); break;
								case 6: std::sscanf(token.c_str(),"%d",&toto_result); break;
							};
						}
					}
					load_data.emplace_back(std::make_tuple(date,place,number,home_team,score,away_team,toto_result));
				}
			}
		}
	}

	constexpr std::size_t size()const noexcept{return load_data.size();}
	
	auto begin()const noexcept{return load_data.begin();}
	auto end()const noexcept{return load_data.end();}

	void print()const noexcept
	{
		for(const csv_tuple& tuple:load_data){
			tuple_displayer::disp(tuple);
			std::cout<<std::endl;
		}
	}
private:
	csv_vector load_data;
private:
	static constexpr std::string& str_deleter(std::string& s,const std::string& target)
	{
		for(std::size_t c=s.find_first_of(target); c!=std::string::npos; c=s.find_first_of(target))
			s.erase(c,1);
		return s;
	}
};

class same_much final{
	csv_consider::csv_vector result;
	std::pair<std::string,std::string> team;
	bool one_team=false;
public:
	template<class Str,std::enable_if_t<std::is_same<std::remove_reference_t<Str>,std::string>{},std::nullptr_t> =nullptr>
	same_much(const csv_consider& csv_cons,Str&& a_team)noexcept
		:team(a_team,"")
	{
		for(const csv_consider::csv_tuple& d_match:csv_cons){
			if(std::get<3>(d_match)==a_team || std::get<5>(d_match)==a_team){
				result.emplace_back(d_match);
			}
		}
		one_team=true;
	}

	template<class Str,std::enable_if_t<std::is_same<std::remove_reference_t<Str>,std::string>{},std::nullptr_t> =nullptr>
	same_much(const csv_consider& csv_cons,Str&& home,Str&& away)noexcept
		:team(home,away)
	{
		std::vector<csv_consider::csv_vector> samemuch(csv_cons.size());
		std::size_t index=0;

		for(const csv_consider::csv_tuple& d_match:csv_cons){
			for(const csv_consider::csv_tuple& d:csv_cons){
				if( (std::get<3>(d_match)==std::get<3>(d) && std::get<5>(d_match)==std::get<5>(d)) ||
					(std::get<3>(d_match)==std::get<5>(d) && std::get<5>(d_match)==std::get<3>(d)) ){
						samemuch[index].emplace_back(d);
				}
			}
			++index;
		}

		for(const auto& much:samemuch){
			if((std::get<3>(much[0])==home && std::get<5>(much[0])==away) ||
				(std::get<3>(much[0])==away && std::get<5>(much[0])==home)){
				std::copy(std::begin(much),std::end(much),std::back_inserter(result));
				break;
			}
		}
	}

	void print()const noexcept
	{
		csv_vector_display(result);
		std::cout<<std::endl;
	}
	
	boost::optional<double> percentages()const noexcept
	{
		if(one_team){
			const std::size_t game_stats=result.size();
			std::size_t win=0;

			for(const csv_consider::csv_tuple& tp:result){
				if((std::get<3>(tp)==team.first && std::get<6>(tp)==1) || (std::get<5>(tp)==team.first && std::get<6>(tp)==2)){
					++win;
				}
			}
			return (static_cast<double>(win)/static_cast<double>(game_stats))*100;
		}
		return {};
	}

	boost::optional<double> average()const noexcept
	{
		if(!one_team){
			boost::accumulators::accumulator_set<std::size_t,boost::accumulators::stats<boost::accumulators::tag::mean>> acc;
			std::vector<std::size_t> toto_results(result.size());
			std::size_t index=0;
			for(const auto& r:result)toto_results[index++]=std::get<6>(r);
			boost::for_each(std::move(toto_results),std::bind(std::ref(acc),std::placeholders::_1));
			return boost::accumulators::extract::mean(acc);
		}
		return {};
	}
private:
	void csv_vector_display(const csv_consider::csv_vector& v)const noexcept
	{
		std::cout<<"--- Result --------------------------------"<<std::endl;
		for(const auto& t_data:v){
			tuple_displayer::disp(t_data);
			std::cout<<std::endl;
		}
		std::cout<<"-------------------------------------------"<<std::endl;
	}
};

std::string input_1team()
{
	std::string a_team;
	std::cout<<"Team name ? >> ";
	std::cin>>a_team;
	return a_team;
}

std::pair<std::string,std::string> input_2team()
{
	std::string home,away;
	std::cout<<"1st Team ? >> ";
	std::cin>>home;
	std::cout<<"2nd Team ? >> ";
	std::cin>>away;
	return std::make_pair(home,away);
}

int main()
{
	csv_consider data(boost::filesystem::path(boost::filesystem::current_path())/"csv/");
	if(!data.file_exists)return -1;
	std::cout<<"Data size: "<<data.size()<<std::endl;

	std::string user_input;	
	while(true){
		std::cout<<"(?/p/s/d/q) >> ";
		std::cin>>user_input;
		if(user_input=="?"){
			std::cout<<"? :display this manual\np :display all loaded game\ns: the game result of your selecting team\nd :the game result of double team mode\nq: quit"<<std::endl; 
		}else if(user_input=="p"){
			data.print();
		}else if(user_input=="s"){
			auto input_result=input_1team();
			same_much sm(data,std::move(input_result));
			sm.print();
			std::cout<<"勝率: "<<sm.percentages().value()<<"%"<<std::endl;
		}else if(user_input=="d"){
			auto input_result=input_2team();
			same_much sm(data,std::move(input_result.first),std::move(input_result.second));
			sm.print();
			std::cout<<"toto 中間値: "<<sm.average().value()<<std::endl;
		}else if(user_input=="q"){
			break;
		}
	};
}
