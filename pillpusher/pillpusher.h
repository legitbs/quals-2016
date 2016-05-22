#ifndef __PILLPUSHER__
#define __PILLPUSHER__

#include "unistd.h"
#include "printf.h"
#include "mmap.h"
#include "malloc.h"
#include "stdlib.h"

typedef struct pill {
	char *name;
	unsigned int dose;
	unsigned int schedule;
	int side_effect_count;
	int sec_max;
	char **side_effects;
	int interactions_count;
	int ic_max;
	char **interactions;
	int treats_count;
	int tc_max;
	char **treats;
} pill;

typedef struct pharmacist {
	char *name;

	/// used to determine which pills they are allowed to serve
	unsigned int certifications;
} pharmacist;

typedef struct pharmacy {
	char *name;
	int pill_cnt;
	int pill_max;
	pill **stock;
	int staff_cnt;
	int staff_max;
	pharmacist **staff;
} pharmacy;

typedef struct patient {
	char *name;
	int symptom_max;
	int symptom_cnt;
	char **symptoms;
	char *scrips;
} patient;

typedef struct meta {
	int pharma_max;
	pharmacy **pharmacies;
	int pat_max;
	patient **patients;
	int doc_max;
	pharmacist **docs;
	int drug_max;
	pill **drugs;
} meta;

char *side_effects[] = {"Internal Bleeding", "Vomiting", "Anal Bleeding", "Itching", "Rash", "Dry Mouth", "Bladder Pain",
	"Irregular Heartbeat", "Back Pain", "Side Pain", "Fever", "Chills", "Cough", "Chest Pain", "Confusion",
	"Convulsions", "Diarrhea", "Difficulty Breathing", "Dizziness", "Headache", "Swelling", "Nausia", "Spasms",
	"Weakness", "Tiredness", "Sores", "Ulcers", "Weight Loss", "Constipation", "Loss in sexual ability", "Twitching", 
	"Clumsiness", "Depression", "Falling", "Hallucination", "Irritability", "Wheezing", NULL };

char *pills[] = {"Abilify", "Acetaminophen", "Acyclovir", "Adderall", "Albuterol", "Aleve", "Allopurinol", "Alprazolam", "Ambien", "Amiodarone", "Amitriptyline", "Amlodipine", "Amoxicillin", "Aricept", "Aspirin", "Atenolol", "Ativan", "Atorvastatin", "Augmentin", "Azithromycin", "Baclofen", "Bactrim", "Bactroban", "Belsomra", "Belviq", "Benadryl", "Benicar", "Biaxin", "Bisoprolol", "Boniva", "Boniva", "Botox", "Breo Ellipta", "Brilinta", "Brintellix", "Bupropion", "Buspar", "Buspirone", "Bydureon", "Bystolic", "Carbamazepine", "Carvedilol", "Celebrex", "Celexa", "Cephalexin", "Cetirizine", "Cialis", "Cipro", "Ciprofloxacin", "Citalopram", "Claritin", "Clindamycin", "Clonazepam", "Clonidine", "Codeine", "Coreg", "Coumadin", "Crestor", "Cyclobenzaprine", "Cymbalta", "Demerol", "Depakote", "Depo-Provera", "Dexamethasone", "Dextromethorphan", "Diazepam", "Diclofenac", "Diflucan", "Digoxin", "Dilantin", "Dilaudid", "Diltiazem", "Diovan", "Diphenhydramine", "Ditropan", "Dopamine", "Doxazosin", "Doxycycline", "Dulera", "DuoNeb", "Effexor", "Effient", "Elavil", "Eliquis", "Enalapril", "Enbrel", "Endocet", "Entresto", "Ephedrine", "EpiPen", "Epogen", "Erythromycin", "Estrace", "Estradiol", "Etodolac", "Evista", "Excedrin", "Exelon", "Exforge", "Ezetimibe", "Famotidine", "Farxiga", "Femara", "Fenofibrate", "Fentanyl", "Ferrous Sulfate", "Fetzima", "Fioricet", "Fish Oil", "Flagyl", "Flexeril", "Flomax", "Flonase", "Flovent", "Fluoxetine", "Focalin", "Folic Acid", "Forteo", "Fosamax", "Furosemide", "Gabapentin", "Gablofen", "Gammagard", "Gamunex", "Gardasil", "Gemfibrozil", "Gemzar", "Genvoya", "Geodon", "Gilenya", "Gilotrif", "Gleevec", "Glipizide", "Glucophage", "Glucotrol", "Glucovance", "Glyburide", "Glyxambi", "Gralise", "Guaifenesin", "Halaven", "Harvoni", "Havrix", "Hcg", "Heparin", "Herceptin", "Hizentra", "Horizant", "Humalog", "Humira", "Humulin", "Humulin N", "Hydrochlorothiazide", "Hydrocodone", "Hydroxychloroquine", "Hydroxyzine", "Hylenex", "Hysingla ER", "Hytrin", "Hyzaar", "Ibrance", "Ibuprofen", "Imbruvica", "Imdur", "Imitrex", "Imodium", "Implanon", "Incruse Ellipta", "Inderal", "Injectafer", "Insulin", "Integrilin", "Intermezzo", "Intuniv", "Invega", "Invokamet", "Invokana", "Iressa", "Isentress", "Isosorbide", "Jakafi", "Jalyn", "Janumet", "Januvia", "Jardiance", "Jentadueto", "Jetrea", "Jevtana", "Jublia", "Juvederm", "Juvisync", "Juxtapid", "K-dur", "Kadcyla", "Kadian", "Kaletra", "Kapvay", "Kazano", "Kcentra", "Keflex", "Kenalog", "Keppra", "Kerydin", "Keytruda", "Kineret", "Klonopin", "Klor-con", "Kombiglyze XR", "Krill Oil", "Krystexxa", "Kyprolis", "Kytril", "Lamictal", "Lansoprazole", "Lantus", "Lasix", "Latuda", "Levaquin", "Levemir", "Levothyroxine", "Lexapro", "Linzess", "Lipitor", "Lisinopril", "Lithium", "Loratadine", "Lorazepam", "Lortab", "Losartan", "Lovenox", "Lunesta", "Lyrica", "Macrobid", "Meclizine", "Melatonin", "Meloxicam", "Metformin", "Methadone", "Methocarbamol", "Methotrexate", "Methylphenidate", "Methylprednisolone", "Metoclopramide", "Metoprolol", "Metronidazole", "MiraLax", "Mirtazapine", "Mobic", "Morphine", "Motrin", "Mucinex", "Myrbetriq", "Naloxone", "Namenda", "Naprosyn", "Naproxen", "Nasacort", "Nasonex", "Neurontin", "Nexium", "Niacin", "Niaspan", "Nicotine", "Nifedipine", "Nitrofurantoin", "Norco", "Nortriptyline", "Norvasc", "NovoLog", "Nucynta", "NuvaRing", "Nuvigil", "Olysio", "Omeprazole", "Omnicef", "Ondansetron", "Onfi", "Onglyza", "Opana", "Opdivo", "Opsumit", "Orapred", "Orencia", "Orlistat", "Ortho Evra", "Ortho Tri-Cyclen", "Oseltamivir", "Osphena", "Otezla", "Oxybutynin", "Oxycodone", "Oxycontin", "Paracetamol", "Paroxetine", "Paxil", "Percocet", "Phenergan", "Phentermine", "Plaquenil", "Plavix", "Potassium Chloride", "Pradaxa", "Pravastatin", "Prednisone", "Premarin", "Prilosec", "Pristiq", "Prolia", "Promethazine", "Propranolol", "Protonix", "Prozac", "QNASL", "Qsymia", "Quaaludes", "Quillivant XR", "Qutenza", "Ramipril", "Ranexa", "Ranitidine", "Rapaflo", "Reglan", "Relafen", "Relpax", "Remeron", "Remicade", "Renvela", "Requip", "Restasis", "Restoril", "Rexulti", "Risperdal", "risperidone", "Ritalin", "Rituxan", "Robaxin", "Rocephin", "Saphris", "Savella", "Saxenda", "Senna", "Sensipar", "Septra", "Seroquel", "Sertraline", "Sildenafil", "Simvastatin", "Singulair", "Skelaxin", "Soma", "Spiriva", "Spironolactone", "Strattera", "Suboxone", "Sudafed", "Symbicort", "Synthroid", "Tamiflu", "Tamoxifen", "Tamsulosin", "Tegretol", "Temazepam", "Terazosin", "Testosterone", "Tetracycline", "Tizanidine", "Topamax", "Toradol", "Tradjenta", "Tramadol", "Trazodone", "Triamcinolone", "Triamterene", "Tricor", "Trileptal", "Trulicity", "Tylenol", "Uceris", "Ulesfia", "Uloric", "Ultane", "Ultracet", "Ultram", "Ultresa", "Uptravi", "Uroxatral", "Utibron Neohaler", "Valacyclovir", "Valium", "Valtrex", "Vancomycin", "Vasotec", "Venlafaxine", "Ventolin", "Verapamil", "Vesicare", "Viagra", "Vicodin", "Victoza", "Viibryd", "Vimovo", "Vimpat", "Vistaril", "Voltaren", "Voltaren Gel", "Vytorin", "Vyvanse", "Warfarin", "Wellbutrin", "Wilate", "Xalatan", "Xalkori", "Xanax", "Xanax XR", "Xarelto", "Xeljanz", "Xeloda", "Xenazine", "Xenical", "Xeomin", "Xgeva", "Xiaflex", "Xifaxan", "Xigduo XR", "Xofigo", "Xolair", "Xopenex", "Xtandi", "Xyrem", "Xyzal", "Yasmin", "Yaz", "Yervoy", "Yondelis", "Zanaflex", "Zantac", "Zestoretic", "Zestril", "Zetia", "Zithromax", "Zocor", "Zofran", "Zoloft", "Zolpidem", "Zometa", "Zomig", "Zostavax", "Zosyn", "Zovirax", "Zubsolv", "Zyprexa", "Zyrtec", "Zytiga", "Zyvox", NULL};

char *symptoms[] = {"Anxiety", "Allergic Rhinitis", "ADHD", "Anxiety and Stress", "Asthma, Maintenance", "Bipolar Disorder", "Back Pain", "Bacterial Infection", "Benign Prostatic Hyperplasia", "Birth Control", "Chronic Pain", "Constipation", "Cough", "COPD", "Depression", "Diabetes", "Dietary Supplementation", "Diabetic Peripheral Neuropathy", "Dental Abscess", "Edema", "Erectile Dysfunction", "Epilepsy", "Erosive Esophagitis", "Eczema", "Fibromyalgia", "Folic Acid Deficiency", "Fever", "Fatigue", "Female Infertility", "GERD", "Generalized Anxiety Disorder", "Gout", "Glaucoma", "Gout", "Galactosemia", "High Blood Pressure", "High Cholesterol", "Heart Failure", "Heart Attack", "Headache", "Insomnia", "Irritable Bowel Syndrome", "Indigestion", "Inflammatory Conditions", "Iron Deficiency Anemia", "Japanese Encephalitis Virus Prophylaxis", "Jaundice", "Jet Lag", "Jock Itch", "Joint Infection", "Joint Pain", "Juvenile Rheumatoid Arthritis", "Junctional Premature Depolarizations", "Keratitis", "Kok disease", "Lactation Augmentation", "Lip Augmentation", "Liposarcoma", "Muscle Spasm", "Migraine", "Migraine Prevention", "Major Depressive Disorder", "Muscle Pain", "Naophyetus salmincola", "Narcolepsy", "Neutropenia", "Nightmares", "Nocturnal Polyuria", "Osteoarthritis", "Osteoporosis", "Overactive Bladder", "Obesity", "Opiate Withdrawal", "Polycythemia", "Dyspareunia", "Postoperative Gas Pains", "Pouchitis", "Quadriplegia", "Quincke\'s disease", "Rheumatoid Arthritis", "Restless Legs Syndrome", "Rhinitis", "Rosacea", "Raynaud\'s Syndrome", "Sarcoidosis", "Scabies", "Schistosoma haematobium", "Schizophrenia", "Sciatica", "Sporotrichosis", "Tendonitis", "Trichomoniasis", "Temporomandibular Joint Disorder", "Thromboembolic Stroke Prophylaxis", "Trigeminal Neuralgia", "Underactive Thyroid", "Urinary Tract Infection", "Upper Respiratory Tract Infection", "Urinary Incontinence", "Urticaria", "Vaginal Dryness", "Varicella Pneumonitis", "Vasculitis", "Vitrectomy", "Vulvodynia", "Watery stools", "West Nile Virus", "Whipple\'s Disease", "Whipworm Infection", "Xerostomia", "Yaws", "Zika Virus Infection", "Zollinger-Ellison Syndrome", "Zygomycosis", NULL};

//// Delcaration parcticularly to handle the frame pointer stuff
/// Trying to hide in the weeds. Omit every other one
int read_delim( int fd, char *buf, char delim, int maxlen );
int read_integer( int fd ) __attribute__((optimize("-fno-omit-frame-pointer")));
void add_pill_to_meta( pill *np);
void add_pill_treatment( pill *np, char *t, int length ) __attribute__((optimize("-fno-omit-frame-pointer")));
void add_pill_interaction( pill *np, char *t );
void add_pill_sideeffect( pill *np, char *t ) __attribute__((optimize("-fno-omit-frame-pointer")));
int check_pill_in_list( char *nm );
int add_pill( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
void print_pill( pill *p );
int list_pills( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
void random_pill( void );
pill *get_pill( char *name ) __attribute__((optimize("-fno-omit-frame-pointer")));
void print_side_effects( pill *p );
void remove_side_effect( pill *p, int index ) __attribute__((optimize("-fno-omit-frame-pointer")));
void add_side_effect( pill *p );
void edit_side_effect( pill *p, int index ) __attribute__((optimize("-fno-omit-frame-pointer"))); //
void set_effect( pill *p, char *data, int index );

/// For the frame pointer to be leakable in edit_side_effect() the parent must also have one.
void update_pill_side_effect( pill *p ) __attribute__((optimize("-fno-omit-frame-pointer")));
void remove_interaction( pill *p, int index );
void add_interaction( pill *p ) __attribute__((optimize("-fno-omit-frame-pointer")));
void edit_interaction( pill *p, int index );
void print_interactions( pill *p ) __attribute__((optimize("-fno-omit-frame-pointer")));
void update_pill_interactions( pill *p );
void print_treatments( pill *p ) __attribute__((optimize("-fno-omit-frame-pointer")));
void remove_treatment( pill *p, int index );
void add_treatment( pill *p ) __attribute__((optimize("-fno-omit-frame-pointer")));
void edit_treatment( pill *p, int index );
void update_pill_treatments( pill *p ) __attribute__((optimize("-fno-omit-frame-pointer")));
void modify_pill( void );
void lose_pills( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
void pill_menu( void );
void add_pill_to_pharmacy( pharmacy *ph, pill *pi ) __attribute__((optimize("-fno-omit-frame-pointer")));
pharmacist *get_doc( char *name );
void list_pharmacists( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
void add_staff_to_pharmacy( pharmacy *ph, pharmacist *doc );
void add_pharmacy_to_meta( pharmacy *ph) __attribute__((optimize("-fno-omit-frame-pointer")));
void create_pharmacy( void );
void remove_pill_from_pharmacy( pharmacy *p ) __attribute__((optimize("-fno-omit-frame-pointer")));
void update_pill_to_pharmacy( pharmacy *p);
void update_pharmacy_pills( pharmacy *p ) __attribute__((optimize("-fno-omit-frame-pointer")));
void remove_doc_from_pharmacy( pharmacy *p );
void update_staff_to_pharmacy( pharmacy *p ) __attribute__((optimize("-fno-omit-frame-pointer")));
void update_pharmacy_staff( pharmacy *p );
void update_pharmacy( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
void list_pharmacies( void );
void delete_pharmacy( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
void pharmacy_menu( void );
void add_pharmacist_to_meta( pharmacist *ph) __attribute__((optimize("-fno-omit-frame-pointer")));
void add_pharmacist( void );
void remove_pharmacist( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
void update_pharmacist( void );
void pharmacist_menu( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
void add_patient_to_meta( patient *p);
void add_symptom_to_patient( patient *p, char *s ) __attribute__((optimize("-fno-omit-frame-pointer")));
void add_patient( void );
void list_patients( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
patient *get_patient( char *name );
void delete_patient( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
void update_patient( void );
void patient_menu( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
pharmacy *get_pharmacy( char *data );
pharmacy * select_pharmacy( void ) __attribute__((optimize("-fno-omit-frame-pointer")));
pharmacist *select_doc( pharmacy *store );
void list_pharmacy_pills( pharmacy *ph) __attribute__((optimize("-fno-omit-frame-pointer")));
long add_pill_to_scrip( pharmacy *ph, pharmacist *doc, patient *pat, char *scrip);
void scrip_menu( void ) __attribute__((optimize("-fno-omit-frame-pointer")));

/// Since the overflow is here I want to make sure that rbp is not pushed
/// Don't want them getting control over that.
void add_scrip( pharmacy *ph, pharmacist *doc, patient *pat);
void main_menu( void ) __attribute__((optimize("-fno-omit-frame-pointer")));

#endif
