#include <omp.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <windows.h>


using namespace std;
using namespace chrono;

// OpenMP de #pragma ... altında kalan ilk blok bu pragmada
// sağlanan özelliklere sahip olur. Bu bloktan sonra kod yine
// normal c++ derleyicisindeki özellikler ile devam eder

// g++ -fopenmp main.cpp -o main kodu bu şekilde derlemelisiniz
// -fopenmp olmadan OpenMP özellikleri derlenmez ve hata verir

int main() {

    // ==========================================================
    //  ÖRNEK 1: OpenMP Parallel Temelleri
    // ==========================================================

    // Bu kod altındaki Hello toplam thread sayısı kadar
    // çağrılır. Örneğin benim bilgisayarım 16 core yani 32 
    // thread aynı anda çalışabiliyor. Bu yüzden 32 kere Hello
    // çağrılır ama World yazısı sadece bir kere çağrılır çünkü o
    // artık farklı bir blokta yer alır.
    #pragma omp parallel
    printf("Hello\n");   
    printf("World\n"); 

    // omp_set_num_threads global bir ayardır yani bundan sonra
    // thread sayımız 4'e sabitlenir. Fakat bundan önceki kodda
    // var olan tüm threadleriniz kullanılmaktaydı.
    omp_set_num_threads(4);

    // Bu kodda ise hem Hello hem World paralel olarak birden fazla
    // thread üzerinde çalıştırılır. 
    #pragma omp parallel
    {
        printf("Hello\n");
        printf("World\n");
    }

    // OpenMP her threade bir sayı atar ama bu operating systemdan
    // gelen threadid değildir. 
    // Thread id ye ulaşmak için işletim sisteminin kendi
    // thread id kodunu kullanmanız gerekir.
    // Benim kodum şu an Windows üzerinde çalıştığı için
    // GetCurrentThreadId fonksiyonu windows.h üzerinden çağrılır.
    #pragma omp parallel
    {
        int omp_id = omp_get_thread_num();
        DWORD os_tid = GetCurrentThreadId();
        printf("OpenMP thread %d -> OS thread ID: %lu\n", omp_id, os_tid);
    }
    

    // ==========================================================
    //  ÖRNEK 2: Static vs Dynamic Schedule Testi
    // ==========================================================

    // Parallel komutu bloktaki her işlemi bütün threadler tarafından
    // baştan sona aynı şekide çalıştırı ama bazen belirli bir 
    // sayıda işlemi farklı threadler tarafından bir kere yapılmasını 
    // istersiniz. Bu durumda parallel for ile bir döngü sadece
    // bir kez tamamlanır ama iterasyonlar threadlere parçalanarak
    // parallel şekilde yürütülür.
    int sum = 0;

    #pragma omp parallel for
    for (int i = 0; i < 10000; i++) {
        #pragma omp atomic
        sum += 1;
    }
 
    printf("Sum is %d\n", sum);

    // Varsayılan olarak döngülerde schedule(static) geçelidir
    // Bu static şu anlama gelir döngü threadlere eşit olarak 
    // bölünür. Overheadi azdır ama karmaşık işlemlerde sıkıntı çıkartabilir
    // schedule(dynamic) ise kim müsait ise ona atama yapar.
    sum = 0;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < 10000; i++) {
        #pragma omp atomic
        sum += 1;
    }

    printf("Sum is %d\n", sum);


    // OpenMP parallel işlemlerde daha güvenli olacak şekilde 
    // tasarlanmış bir monoton artan saat kullanır ve bu saat OS saati ile 
    // senkronize olmadığından kullanıcı tarafından bozulamaz.
    // omp_get_wtime() fonksiyonu o anki ölüçümü getirir ve ana amacı
    // iki farklı zamanı birbirinden çıkartarak geçen zamanı  hesaplamaktır

    // Burda wtime wall-clock time olarak geçer ve ölçtüğü şey
    // kullanıcının gerçekte beklediği süredir.
    // Eğer CPU kullanım oranı istenirse clock() gibi fonksiyonlar
    // kullanılmalıdır.
    double start = omp_get_wtime();

    int a = 0;
    #pragma omp parallel for
    for (int i = 0; i < 10000; i++) {
        #pragma omp atomic
        a += 1;
    }
    printf("Counters final value is: %d \n", a);
    double end = omp_get_wtime();

    printf("Elapsed time = %f seconds\n", end - start);
}