## Yandex Practicum C++

## Модуль 1 - Основные компоненты С++

![изображение](https://user-images.githubusercontent.com/110821533/235262881-bdfa2034-d4e5-4ab6-b034-5e8d2bba2bfc.png)
 [sprint-1](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_1) - (основа поисковой системы)
 
![изображение](https://user-images.githubusercontent.com/110821533/235262908-ff45f781-cb1e-4741-8c0c-1f81c7001825.png)
 [sprint-2](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_2) - (улучшение результатов поиска спомощью статической мере TD-IDF-https://ru.wikipedia.org/wiki/TF-IDF)
 
![изображение](https://user-images.githubusercontent.com/110821533/235262912-372f001c-0332-442b-8875-b24e135227a4.png)
 [sprint-3](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_3) - (добавление фреймворка автотестирования поисковой системы)
 
![изображение](https://user-images.githubusercontent.com/110821533/235262919-040b8fcb-c76c-48b8-b794-8dcb85eedcc5.png)
 [sprint-4](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_4) - (обработка исключений поисковых запросов включая ООП + перегрузки)
 
![изображение](https://user-images.githubusercontent.com/110821533/235262927-b12b4879-1ac7-49d8-ad8e-ac5824a181ce.png)
 [sprint-5](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_5) - (Добавление в посковую систему итератов + алгоритмов + стека/очереди/дэка + разбиение проекта на файлы)
 

## Модуль 2 - Эффиктивность кода 

![изображение](https://user-images.githubusercontent.com/110821533/235262930-e0b07ff9-9026-4daf-8470-98b42ed650b9.png)
 [sprint-6](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_6) - (добавление депупликации документа + профилирование + оптимизация + асимптотическая сложность алгоритмов)
 
![изображение](https://user-images.githubusercontent.com/110821533/235262935-26992f54-bb17-46d5-8d5e-db86d18b50c2.png)
[sprint-7](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_7) - (Создание своего односвязного списка)

![изображение](https://user-images.githubusercontent.com/110821533/235262942-828f56f5-17fd-4a15-8f7c-445fca0f0164.png)
 [sprint-8](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_8) - (Создание собственного контейнера "vector")
 
![изображение](https://user-images.githubusercontent.com/110821533/235262955-bdb7def7-47c2-487a-b8d0-2e468e1de4d2.png)
 [sprint-9](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_9) - (Итоговая версия поиского сервера (добавление паралеливания запросов посиковой системы + паралеливание методов + паралеливание поиска докупентов))
 

 ## Модуль 3 - Элегантность и маштабируемость 
 

![изображение](https://user-images.githubusercontent.com/110821533/235262978-e0f43987-16e3-48eb-a478-301a65de7b53.png)
 [sprint-10](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_10) - (основа транспортно справочника)
 
![изображение](https://user-images.githubusercontent.com/110821533/235262881-bdfa2034-d4e5-4ab6-b034-5e8d2bba2bfc.png) [sprint-11](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_11) - (добавление визуализации карты маршрутов транспортного справочника (конечная версия - ![изображение](https://user-images.githubusercontent.com/110821533/235263240-4ba273c5-59cb-4d5e-a8dd-fc297889467c.png)
 
нынешнаяя версия - 

![изображение](https://user-images.githubusercontent.com/110821533/235263592-abbc0211-90b2-4e2c-99a1-17adf7eb73c7.png)



![изображение](https://user-images.githubusercontent.com/110821533/235262881-bdfa2034-d4e5-4ab6-b034-5e8d2bba2bfc.png) [sprint-12](https://github.com/kqaba/yandex_cpp/tree/main/src/sprint_12) - (Добавление вспомогательной библиотеки для построения JSON с помощью цепочек вызова методов для транспортного справочника)


## Посиквой сервер

Функции поискового сервера 

 * ранжирование результатов поиска по статистической мере TF-IDF;
 
 * обработка стоп-слов (не учитываются поисковой системой и не влияют на результаты поиска);
 
 * обработка минус-слов (документы, содержащие минус-слова, не будут включены в результаты поиска);
 
 * создание и обработка очереди запросов;
 
 * удаление дубликатов документов;
 
 * постраничное разделение результатов поиска;
 
 * возможность работы в многопоточном режиме;



