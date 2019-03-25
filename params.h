
#define BASE_PERIOD     333
#define OMEGA           (2 * PI / BASE_PERIOD)
#define MANY_YEARS      10

/* Range di oscillazione */

#define BASE_RANGE          0.33    /* Base */
#define SOLAR_RANGE         0.15    /* Sulla base */
#define TEMPERATURE_RANGE   0.09    /* Sul solare */
#define AIR_RANGE           0.18    /* Sulla temperatura */
#define WATER_RANGE         0.12    /* Sulla temperatura */
#define GROWTH_RANGE        0.07    /* Sulla temperatura */

#define AVERAGE_GROUND_FACTOR   0.50
#define DELTA_GROUND_FACTOR     (200.0 / GROUND_AREA)

#define RHO                 0.1     /* Rapporto tra usura a regime e usura nei transitori */
#define CONTROL_TOLL        0.05    /* Vincoli stringenti a meno di un 5% sono considerati ~= */
#define USER_EPS            0.01

#define BARRIER_RESOURCE    heat
#define BARRIER_FACTOR      0.03    /* Con 100 di throughput il 95% di protezione */

#define HABITAT_DEFAULT_PEOPLE      20
#define HABITAT_MAX_PEOPLE          60
#define HABITAT_AUTONOMY_DAYS       75      /* Con default people e eta = 1 */
#define HABITAT_MAX_CAPACITY        (HABITAT_AUTONOMY_DAYS * HABITAT_DEFAULT_PEOPLE)
#define HYBRIDOME_MAX_PEOPLE        45
#define HYBRIDOME_MAX_CAPACITY      300

#define SHUTTLE_STAY        1
#define MIN_TIME_GRANTED    10  /* Sopra questa soglia la speranza di vita diviene aleatoria */

#define PANTROPIC_STEPS_TO_BREED    (BASE_PERIOD / 3)
#define ALIEN_STEPS_TO_BREED        (BASE_PERIOD / 5)

#define MUTATION_RATIO          0.1
#define ALIEN_FORMS_RATIO       0.2
#define GIZMO_FACTOR            (1.0 / BASE_PERIOD)

#define INITIAL_PARASITES       20
#define INITIAL_BREEDERS        1
#define INITIAL_GIZMOS          1
#define INITIAL_MOLES           1

#define INITIAL_MONEY       5000

#define LIFE_KAPPA              0.1     /* Tasso di morte */
#define BIRTH_RATIO             0.2     /* LIFE_KAPPA * BIRTH_RATIO = Tasso di nascita */
#define HYBRID_SUPPORT_MARGIN   1.5
/* Da 1 in su vale: hybrid growing <= MARGIN * available hybrid support */

#define MONEY_VALUE         0.001
#define HUMANS_VALUE        1
#define HYBRIDS_VALUE       10
#define PANTROPICS_VALUE    100

#define REPAIR_FACTOR           1.0
#define CRITICAL_REPAIR_FACTOR  (3 * REPAIR_FACTOR)

