#include <iostream>
#include <string>
#include <numeric>
#include <vector>
#include "MillerRabin.h"
#include <boost/multiprecision/cpp_int.hpp>
//#include <boost/multiprecision/miller_rabin.hpp>


using namespace std;

using boost::multiprecision::cpp_int;
using namespace boost::multiprecision;
using namespace boost::random;


class KeyGeneration{
public:
  	boost::multiprecision::cpp_int p; // a prime input
  	boost::multiprecision::cpp_int q; // a prime input
  	boost::multiprecision::cpp_int n; // n = p*q
  	boost::multiprecision::cpp_int phi_n;// phi_n = (p-1)*(q-1)
  	boost::multiprecision::cpp_int e; //1< pyi_n; e and phi_n coprime
  	boost::multiprecision::cpp_int d=0; //(e*d) mod phi_n = 1
  	string mes = "";//secret message
  	vector<boost::multiprecision::cpp_int> mes_Cript={};//ciphertext
  	vector<boost::multiprecision::cpp_int> mes_0={};//result of decryption
  
  	void prompter();
  	cpp_int calculate_n();
  	cpp_int calculate_phi();
  	unsigned calculate_e();
  	cpp_int GetPrime();
  	cpp_int calculate_d (cpp_int a, cpp_int b1, cpp_int &x , cpp_int &y ) ;
  	void block_crypt();
  	void block_decrypt();
  
};

//получаем  случайные простые числа заданного размера (например, 1024 бита каждое);
cpp_int KeyGeneration::GetPrime(){
	mt11213b base_gen(clock());
  	independent_bits_engine<mt11213b, 256, cpp_int> gen(base_gen);
	cpp_int t;
  	do
  	{
		t = gen();
	}while (!Miller_rabin_test(t, 25));
	return t;
}
//записываем два различных случайных простых числа  p и q  
void KeyGeneration::prompter(){
	p = GetPrime();
	q = GetPrime();
}
//вычисляется их произведение n = p ⋅ q, которое называется модулем;
cpp_int KeyGeneration::calculate_n(){
	n = p*q;
	return n;
}
//вычисляется значение функции Эйлера от числа n : 
cpp_int KeyGeneration::calculate_phi(){
	phi_n = (p-1)*(q-1);
	return phi_n;
}
//выбирается целое число e ( 1 < e < φ ( n )), взаимно простое со значением функции φ(n) 
unsigned KeyGeneration::calculate_e(){
	cpp_int exp = GetPrime();
  	if(exp < phi_n && gcd(exp,phi_n) == 1){
  		e = (unsigned)exp;
  		return (unsigned)exp;
  	}
  	else 
  		calculate_e(); 
  	return -1;
}
//вычисляется число d, мультипликативно обратное к числу e по модулю φ(n),
// расширенный евклид
cpp_int KeyGeneration::calculate_d (cpp_int a, cpp_int b, cpp_int &x , cpp_int &y ) {
  	if (a == 0) {
		x = 0; y = 1;
    		return b;
  	}
  	cpp_int x1, y1;
  	cpp_int d1 = calculate_d (b%a, a, x1, y1);
  	x = y1 - (b / a) * x1;
  	y = x1;
  	if(d1 == 1)
    		d = (x % b + b) % b;
  	return d1;
}
//Перед шифрованием, мы применяем к сообщению правило: b := (b + a) % n. Где a — предыдущая часть сообщения, а b — следующая. а потом шифруем по правилу (mes^e)%n
void KeyGeneration::block_crypt(){
	mes_Cript.push_back(GetPrime()); 	
	for(int i=0; i < mes.length(); i++) 
		mes_Cript.push_back(( mes_Cript[i] + (cpp_int)mes[i]) % n);
  	for(int i=1;i < mes_Cript.size();i++)
		mes_Cript[i]=modexp(mes_Cript[i],e,n);
} 
//Тот, кто получит ваше сообщение, должен будет проделать обратную операцию, со знаком «минус»: b := (b - a) % n и расшифровываем по правилу (mes_Cript^d)%n
void KeyGeneration::block_decrypt(){
	mes_0.push_back(mes_Cript[0]);
	for(int i=1;i<mes_Cript.size();i++)
  		mes_0.push_back(modexp(mes_Cript[i],d,n));
  	for(int i=mes_0.size()-1;i>0;i--) 
  		mes_0[i] = (mes_0[i] - mes_0[i-1])% n; 		
} 
//
int main(int argc,const char * argv[]){
	try{	
		KeyGeneration obj1;
		obj1.prompter();//получаем два случайные простые числа
		cout<<"\nq = "<<obj1.q << " p = "<<obj1.p;
		cout<<"\nвычисленное произведение n = "<<obj1.calculate_n();
		cout<<"\nфункция Эйлера phi = "<<obj1.calculate_phi();
		cout<<"\nоткрытый экспонента e = "<<obj1.calculate_e();
		cpp_int x, y;
		obj1.calculate_d(obj1.e, obj1.phi_n, x, y);
		cout<<"\nсекретный экспонента d = "<< unsigned(obj1.d);
		cout<<"\nоткрытый ключ {e,n} = {"<<unsigned(obj1.e)<<","<<obj1.calculate_n()<<"}";
		cout<<"\nзакрытый ключ {d,n} = {"<<unsigned(obj1.d)  <<"," << obj1.n << "}";
		
		cout<<"\n\nВведите сообщение: ";
  		getline(cin,obj1.mes);
  		
  		cout << "\nЗашифрованное сообщение  = ";
  		obj1.block_crypt();//шифруем данный нам текст
  		for(int i=0;i<obj1.mes_Cript.size();i++)
  			cout<<obj1.mes_Cript[i]<<" ";
  			
  		cout<< "\nРасшифрованное сообщение  = ";
  		obj1.block_decrypt();//расшифровываем наш зашифрованный текст 
  		for(int i=0;i<obj1.mes_0.size();i++)
  			cout<<(char)obj1.mes_0[i];
	}
	catch(runtime_error & s){
		cout<<s.what()<<endl;
	}
	return 0;
}