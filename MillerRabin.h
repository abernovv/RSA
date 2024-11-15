#include <boost/random.hpp>
#include <boost/multiprecision/integer.hpp>

using namespace boost::multiprecision;

//быстрое возведение в степень (х^y)%N
boost::multiprecision::cpp_int modexp(cpp_int x,cpp_int y,cpp_int N){
  	if (y == 0) return 1;
  	cpp_int z = modexp(x, y / 2, N);
  	if (y % 2 == 0)  return (z*z) % N;
  	else  return (x*z*z) % N;
}

// тест Миллера — Рабина на простоту числа
// производится k раундов проверки числа n на простоту
bool Miller_rabin_test(cpp_int n,int k){
// если n == 2 или n == 3 - эти числа простые, возвращаем true
	if (n==2 || n==3)
		return true;
// если n < 2 или n четное - возвращаем false	
	if(n<2 || n%2==0)
		return false;
// представим n − 1 в виде (2^s)·t, где t нечётно, это можно сделать последовательным делением n - 1 на 2
	cpp_int t = n-1;
	int s=0;
	while(t%2==0){
		t/=2;
		s+=1;
	} 
// повторить k раз
	for(int i=0;i<k;i++){
	// выберем случайное целое число a в отрезке [2, n − 2]
	//mt19937 это генератор мерсенна, который генерирует необработанные случайные числа.
	 // rng является экземпляром генератора твистера.
	 boost::mt19937 rng;
	 //one является экземпляром дистрибутива . Это определяет числа, которые мы хотим сгенерировать
      	boost::uniform_int one;
      	boost::variate_generator< boost::mt19937, boost::uniform_int<> > dice(rng, one);
        // dice это то, что берет необработанные числа и распределение и создает для нас числа, которые мы действительно хотим.
        cpp_int a = (2+dice()) % n-2;
// x ← a^t mod n, вычислим с помощью возведения в степень по модулю
	cpp_int x = modexp(a,t,n);
	
// если x == 1 или x == n − 1, то перейти на следующую итерацию цикла
		if(x==1||x==n-1)continue;
		
		for(int r=1;r<s;r++)
		{
		// x ← x^2 mod n
			x = modexp(x,2,n);
			 // если x == 1, то вернуть "составное"
			if(x==1) return false;
			 // если x == n − 1, то перейти на следующую итерацию внешнего цикла
            
			if(x==n-1)break;
		}
		if(x!=n-1)
			return false;
	}
	// вернуть "вероятно простое"
	return true;
}

