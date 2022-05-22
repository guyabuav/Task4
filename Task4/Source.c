#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX 1023

typedef struct StudentCourseGrade
{
	char courseName[35];
	int grade;
}StudentCourseGrade;

typedef struct Student
{
	char name[35];
	StudentCourseGrade* grades; //dynamic array of courses
	int numberOfCourses;
}Student;


//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);

//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
Student* readFromBinFile(const char* fileName);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);


int main()
{
	//Part A
	int* coursesPerStudent = NULL;
	int numberOfStudents = 0;
	countStudentsAndCourses("studentList.txt", &coursesPerStudent, &numberOfStudents);
	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Discrete Mathematics", +10);
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	//studentsToFile(students, coursesPerStudent, numberOfStudents); ///this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)

	//Part B
	Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("studentsWRITE.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("studentsWRITE.bin");
	
	
	
	

	//add code to free all arrays of struct Student


	//_CrtDumpMemoryLeaks(); //uncomment this block to(t == *(numberOfStudents)-1 check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	FILE* s_info;
	s_info = fopen(fileName, "rt");
	if (s_info == NULL)
	{
		printf("allocation failed");
		return -1;
	}
	int counter = 0;
	char arrtemp[MAX];
	while (!feof(s_info))
	{
		if (fgets(arrtemp, 1023, s_info) != NULL)
			counter++;
	}
	*numberOfStudents = counter;
	rewind(s_info);
	int* arr = (int*)malloc(sizeof(int) * counter);
	*coursesPerStudent = arr;
	int i = 0;
	while (!feof(s_info))
	{
		if (fgets(arrtemp, sizeof(arrtemp), s_info) != NULL)
		{
			arr[i] = countPipes(arrtemp, sizeof(arrtemp));
			i++;
		}
	}
	fclose(s_info);
	
}
	

int countPipes(const char* lineBuffer, int maxCount)
{
	if (lineBuffer == NULL)
		return -1;
	if (maxCount <= 0)
		return 0;
	int numOfPipes = 0;
	int i = 0;
	while (lineBuffer[i] != 0) {
		if (lineBuffer[i] == '|')
			numOfPipes++;
		i++;
	}
	return numOfPipes;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	FILE* s_info;
	s_info = fopen("studentList.txt", "r");
	countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents); // get the values of the textfile
	char*** students = (char***)malloc(sizeof(char**) * *(numberOfStudents)); // memory allocation for students
	if (students == NULL) {
		printf("Allocation failed");
	}
	for (int i = 0; i < *(numberOfStudents); i++) { // memory allocation for name,courses and grades
		students[i] = (char**)malloc((((sizeof(char*) * 2) * (*(numberOfStudents))) )+ 1);
		if (students[i] == NULL) {
			printf("Allocation failed");
		}
	}
	char str[MAX];
	char* token = str;
	int LengthOfToken, i = 0, j = 0;
	while (!feof(s_info)) {
		fgets(str, MAX, s_info); //get the first line from the text
		token = strtok(str, "|,"); // get the first word in first line in text (name)
		while (token != NULL) {
			LengthOfToken = strlen(token); // count str size for memory allocation
			*(students[i] + j) = (char*)malloc((LengthOfToken * sizeof(char)) + 1);
			if (*(students[i] + j) == NULL) {
				printf("Allocation failed");
			}
			strcpy(*(students[i] + j), token);
			j++;
			token = strtok(NULL, "\n|,"); // keep on line to get all the grades+courses
		}
		j = 0;
		i++;
	}
	fclose(s_info);
	return students;
}


void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	char eden[3];
	int y = 0;
	int r = 0;
	int s = 1;  // equals 1 in order to not check the name of the student when searching for the name of the course. 
	if (factor > 20 || factor < -20) {
		return;
	}
	while (y < numberOfStudents) {  // will run on amount students.
		while (s < coursesPerStudent[r] * 2 + 1) {  // will run on each students list of courses
			if (strcmp(*(students[y] + s), courseName) == 0) {  // if name of course matches the course we are searching for.
				s += 1;  // progress to the next slot in the list where the grade is.
				int grade = atoi(*(students[y] + s));  // define grade as the integer of the grade that needs changing. 
				grade += factor;  // add factor to the grade of the course. 
				if (grade > 100) {  // check if grade has past the top limit. 
					grade = 100;
				}
				if (grade < 0) {  // check if grade has past the bottom limit.
					grade = 0;
				}

				_itoa(grade, eden, 10);  // turn grade back in to a char.

				strcpy(*(students[y] + s), eden);

				//students[y][s] = eden;
				break;  // if course was found, there is no point in checking the same student again.
			}
			s++;
		}
		r++;
		y++;
		s = 1;
	}
}

void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
	for (int i = 0; i < numberOfStudents; i++)
	{
		printf("name: %s\n*********\n", students[i][0]);
		for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
		{
			printf("course: %s\n", students[i][j]);
			printf("grade: %s\n", students[i][j + 1]);
		}
		printf("\n");
	}
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	FILE* s_info;
	s_info = fopen("studentList1.txt", "w+");
	for (int i = 0; i < numberOfStudents; i++) {
		for (int j = 0; j < ((coursesPerStudent[i]) *2) + 1; j++) {
			fputs(students[i][j], s_info); // print student name, courses and grades by order
			fputc('|', s_info); // print | after every element
		}
		fputc('\n', s_info); // print \n after every student
	}
	fclose(s_info); // update new textfile
	free(students);
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	FILE* s_info;
	s_info = fopen("studentsWRITE.bin", "wb");
	fwrite(&numberOfStudents, sizeof(int), 1, s_info);
	for (int i = 0; i < numberOfStudents; i++) {
		fwrite(students[i].name, sizeof(char) * 10, 1, s_info);
		fwrite(&students[i].numberOfCourses, sizeof(int), 1, s_info);
		int tempo = students[i].numberOfCourses;
		for (int j = 0; j < students[i].numberOfCourses; j++) {
			//fwrite(students[i].grades[j].courseName, sizeof(char) * 35, 1, s_info);
			//fwrite(&students[i].grades[j].grade, sizeof(int) , 1, s_info);
			fwrite(&students[i].grades[j], sizeof(StudentCourseGrade), 1, s_info);
		}
	}



	fclose(s_info);
}

Student* readFromBinFile(const char* fileName)
{
	FILE* s_info;
	s_info = fopen("students.bin", "rb");
	if (s_info == NULL) {
		printf("allocation failed");
		return -1;
	}
	int StudentNumber;
	int CoursesNumber;
	fread(&StudentNumber, sizeof(int), 1, s_info);
	Student* stu = (Student*)malloc(sizeof(Student) * StudentNumber); // memory allocation for stu array
	if (stu == NULL) {
		printf("allocation failed");
		return -1;
	}
	for (int i = 0; i < StudentNumber; i++) {
		fread(stu[i].name, sizeof(char) * 35, 1, s_info); // read name of student
		fread(&stu[i].numberOfCourses, sizeof(int), 1, s_info); // read number of courses per student
		stu[i].grades = (StudentCourseGrade*)malloc(sizeof(StudentCourseGrade) * stu[i].numberOfCourses);
		// memory allocation for grades struct
		for (int j = 0; j < stu[i].numberOfCourses; j++) { 
			fread(&(stu[i].grades[j]), sizeof(StudentCourseGrade), 1, s_info); // insert course and grade
			
		}
		
		
	}
	fclose(s_info);
	return stu;
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	Student* stu = (Student*)malloc(numberOfStudents *sizeof(Student)); //memo allocation
	if (stu == NULL) {
		printf("Allocation failed");
		return -1;
	}
	for (int i = 0; i < numberOfStudents; i++) { // put the names by order in struct
		strcpy(stu[i].name ,students[i][0]); // insert student name
		stu[i].numberOfCourses = coursesPerStudent[i]; // insert number of courses per student
		// memory allocation for grades (grades+courses)
		stu[i].grades = (StudentCourseGrade*)malloc(coursesPerStudent[i] * 2 * sizeof(StudentCourseGrade) + 1);
		if (stu[i].grades == NULL) {
			printf("allocation failed");
			return -1;
		}
		for (int j = 1, t = 2, p = 0; j < (coursesPerStudent[i]) * 2; j+= 2, t+=2, p++) { // j for courses, t for grades
			strcpy(stu[i].grades[p].courseName, students[i][j]); // insert course name
			stu[i].grades[p].grade = atoi(students[i][t]); // insert grades
		}
		
	}
	return stu;
}
