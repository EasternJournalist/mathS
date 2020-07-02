#pragma once

#include <MathObject.h>
#include <string>

/*
	In this file, we introduce methods that parse text input of expressions 
	and transform them into MathObject.

	
*/
namespace mathS
{
	class Parser
	{
	public:


		// ���������ַ��� str
		MathObject* Parse(const std::string& str);

	private:
		// �����ж��ַ����͵Ĺ��ߺ���
		static bool inline is_figure(char c);
		static bool inline is_op(char c);
		static bool is_varchar(char c);

		// �����ı����ʽ str�� ��λ�� i ��ʼ�� ���������س�һ�����𲻳���lv��һ������.
		// ע�⣬i�����á�����ʱ�� i ָ��ʶ��Ķ���֮���һ��λ�á�����Է���������ĺ������ż������в�����
		MathObject* Parse(const std::string& str, int& i, int lv);

		// ʶ���ض��Ļ�������ĺ���. ��ͨ����ʶ�𲻳����ض�����ģ�
		MathObject* parse_number(const std::string& str, int& i);
		MathObject* parse_variable(const std::string& str, int& i);
		MathObject* parse_string(const std::string& str, int& i);
		MathObject* parse_vector(const std::string& str, int& i);
		MathObject* parse_function(const std::string& str, int& i);
		MathObject* parse_list(const std::string& str, int& i);

		MathObject* parse_list_forced(const std::string& str, int& i);


		/* 
		ע��parser�Ǻ�ʱ�᷵�� nullptr����ʱ���� MathErrorObject:

		* ���ַǷ��﷨ʱ��parse���ص���MathErrorObject���������﷨�������Ϣ�����������MathErrorObject���յ��÷���ֵ��ĸ��������ֹͣ���������Ѵ�����Ϣ������ȥ
		* ������parse_numberʶ�𵽵Ĳ������֣����Ǳ���������һ���������﷨���᷵��nullptr. �յ��÷���ֵ��ĸ���������������������parser������
		
		*/
	};
}