# Creates a .js script to insert records of Stolen_Vehicle

import random
import json
import string

def create_insert(collection, bson):
	return 'db.{}.insert({})'.format(collection, bson)

def create_bson(reg, name, last_name, make, model):
	obj = {}
	obj['Vehicle_Registration'] = reg
	obj['Vehicle_Owner'] = name + ' ' + last_name
	obj['Vehicle_Make'] = make
	obj['Vehicle_Model'] = model
	return json.dumps(obj)
	

def gen_registration():
	return ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))

# 
def gen_record(names, last_names, makers, models):
	# generate random registration
	reg = gen_registration()		

	# pick fields randomly
	name = names[random.randint(0, len(names)-1)] 
	last_name = last_names[random.randint(0, len(last_names)-1)] 
	maker = makers[random.randint(0, len(makers)-1)] 
	model = models[random.randint(0, len(models)-1)]   
	return create_bson(reg, name, last_name, maker, model)


# Configuration
record_count = 1000000
count = 0

# Open data bases
names = open('names').read().splitlines()
last_names = open('last_names').read().splitlines()
makers = open('makers').read().splitlines() 
models = open('models').read().splitlines()  

db_script = open('script.js', 'w') 

for i in range(0,record_count):
	if i % 100 == 0:
		percentage = 100.0*float(i)/float(record_count)
		print ('Progress: {}%. {} out of {}'.format(percentage, i, record_count))
	bson = gen_record(names, last_names, makers, models)
	insert_command = create_insert("Stolen_Vehicles", bson) + '\n'
	db_script.write(insert_command)
