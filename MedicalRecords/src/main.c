#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

typedef struct medical_record
{
	char* name;
	int id, birth_year;
	float weight, height;
} MedicalRecord;

typedef struct medical_vector
{
	size_t size;
	size_t count;
	MedicalRecord** data;
	void(*Resize)(void* self, unsigned int offset);
	void(*Push)(void* self, MedicalRecord* patient);
	//void(*Pop)(void* self, int index);
	void(*Sort)(void* self);
	MedicalRecord*(*Find)(void* self, unsigned int patientNumber);
} MedicalVector;

void resize_medical_vector(MedicalVector* self, unsigned int offset)
{
	self->data = (MedicalRecord**)realloc(self->data, (self->size + offset) * sizeof(MedicalRecord*));
	self->size += offset;
}

void push_medical_vector(MedicalVector* self, MedicalRecord* patient)
{
	self->Resize(self, sizeof(patient));
	self->data[self->count] = patient;
	self->count += 1;
}

void pop_medical_vector(MedicalVector* self, int index)
{
	self->data[index] = NULL;
	// Move the data down
	self->Resize(self, sizeof(MedicalRecord*));
	self->count -= 1;
}

// TODO: Change From Insertion Sort
void sort_medical_vector(MedicalVector* self)
{
	for (int c = 1; c <= self->count - 1; c++)
	{
		int d = c;
		while (d > 0 && self->data[d - 1]->id > self->data[d]->id)
		{
			MedicalRecord* temp = self->data[d];
			self->data[d] = self->data[d - 1];
			self->data[d - 1] = temp;

			d--;
		}
	}
}

MedicalRecord* find_medical_vector(MedicalVector* self, unsigned int patientId)
{
	MedicalRecord* result = NULL;
	bool found = false;
	int current = self->count / 2;
	int min = 0, max = self->count;
	int amount = 0;

	while (!found && amount <= self->count)
	{
		if (patientId == self->data[current]->id)
		{
			result = self->data[current];
			found = true;
		}
		else if (patientId < self->data[current]->id)
		{
			max = current;
			current = (min + max) / 2;
		}
		else if (patientId > self->data[current]->id)
		{
			min = current;
			current = (min + max) / 2;
		}

		amount++;
	}

	return result;
}

void print_patient(MedicalRecord* record)
{
	printf("-----------------------------------------\n");
	printf("%s, %d, %d, %0.2f kg, %0.2f cm\n", record->name, record->id, record->birth_year, record->weight, record->height);
	printf("-----------------------------------------\n");
}

void print_patients_id(MedicalVector* vector)
{
	for (int i = 0; i < vector->count; i++)
	{
		printf("Patient %d\n", vector->data[i]->id);
	}
}

void create_patient(MedicalVector* vector)
{
	MedicalRecord* med_rec = malloc(sizeof(MedicalRecord));
	med_rec->id = rand();
	
	printf("What is the patients name: \n");
	med_rec->name = malloc(sizeof(char) * 100);
	scanf("%s", med_rec->name);
	getchar();

	printf("What is the patients birth year: \n");
	scanf("%d", &med_rec->birth_year);
	getchar();
	
	printf("What is the patients weight (KG): \n");
	scanf("%f", &med_rec->weight);
	getchar();
	
	printf("What is the patients height (CM): \n");
	scanf("%f", &med_rec->height);
	getchar();

	vector->Push(vector, med_rec);
}

void save_patients(MedicalVector* vector, char* filename)
{
	strcat(filename, ".mdr");
	FILE* f = fopen(filename, "ab+");
	if (f == NULL)
		printf("Error finding file!\n");
	
	char* text = malloc(sizeof(vector->data));
	for (int i = 0; i < vector->count; i++)
	{
		if (i == 0)
			strcpy(text, vector->data[i]->name);
		else
			strcat(text, vector->data[i]->name);

		strcat(text, ":");
		strcat(text, vector->data[i]->id);
		strcat(text, ":");
		strcat(text, vector->data[i]->birth_year);
		strcat(text, ":");
	}

	fwrite(text, sizeof(text[0]), sizeof(text) / sizeof(text[0]), f);
	fclose(f);
}

int main()
{
	srand(time(NULL));

	MedicalVector vector;
	vector.size = 0;
	vector.count = 0;
	vector.data = (MedicalRecord**)malloc(0 * sizeof(MedicalRecord*));
	vector.Resize = &resize_medical_vector;
	vector.Push = &push_medical_vector;
	//vector.Pop = &pop_medical_vector;
	vector.Sort = &sort_medical_vector;
	vector.Find = &find_medical_vector;
	
	bool exit = false;
	char* userInput = malloc(sizeof(char) * 20);
	while (!exit)
	{
		printf("To View Patient Type 'View', To Add Patient Type 'Add', To Exit Type 'Exit'\n");
		scanf("%s", userInput);
		getchar();

		if (strcmp("Add", userInput) == 0)
		{
			create_patient(&vector);
			vector.Sort(&vector);
		}

		if (strcmp("View", userInput) == 0)
		{
			int num = 0;
			printf("Type in the patient number\n");
			scanf("%d", &num);
			getchar();
			print_patient(vector.Find(&vector, num));
		}

		if (strcmp("Save", userInput) == 0)
		{
			printf("Give the file a name\n");
			scanf("%s", userInput);
			getchar();
			save_patients(&vector, userInput);
		}

		// TODO: Poping
		//if (strcmp("Delete", userInput) == 0)
		//	vector.Pop(&vector, 1);

		if (strcmp("Exit", userInput) == 0)
			exit = true;

		printf("----------------------\n");
		printf("Patients: \n");
		print_patients_id(&vector);
		printf("----------------------\n");
	}

	free(userInput);
}