//
// Created by kir on 09.05.2025.
//
#include "ArcGamma.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>

// Точность и лимиты
#define GAMMA_EPSILON 1e-12
#define MAX_ITERATIONS 1000
#define STIRLING_COEFF (4.44288293816) // sqrt(2*M_PI)

#ifndef M_GAMMA
/** Euler's constant in high precision */
#define M_GAMMA 0.5772156649015328606065120900824024
#endif

double Kncoe[] = {.30459198558715155634315638246624251,
                  .72037977439182833573548891941219706,
                  -.12454959243861367729528855995001087,
                  .27769457331927827002810119567456810e-1,
                  -.67762371439822456447373550186163070e-2,
                  .17238755142247705209823876688592170e-2,
                  -.44817699064252933515310345718960928e-3,
                  .11793660000155572716272710617753373e-3,
                  -.31253894280980134452125172274246963e-4,
                  .83173997012173283398932708991137488e-5,
                  -.22191427643780045431149221890172210e-5,
                  .59302266729329346291029599913617915e-6,
                  -.15863051191470655433559920279603632e-6,
                  .42459203983193603241777510648681429e-7,
                  -.11369129616951114238848106591780146e-7,
                  .304502217295931698401459168423403510e-8,
                  -.81568455080753152802915013641723686e-9,
                  .21852324749975455125936715817306383e-9,
                  -.58546491441689515680751900276454407e-10,
                  .15686348450871204869813586459513648e-10,
                  -.42029496273143231373796179302482033e-11,
                  .11261435719264907097227520956710754e-11,
                  -.30174353636860279765375177200637590e-12,
                  .80850955256389526647406571868193768e-13,
                  -.21663779809421233144009565199997351e-13,
                  .58047634271339391495076374966835526e-14,
                  -.15553767189204733561108869588173845e-14,
                  .41676108598040807753707828039353330e-15,
                  -.11167065064221317094734023242188463e-15};

double digamma(double x) {
  if (x < 0.0)
    return digamma(1.0 - x) +
           M_PI / tan(M_PI * (1.0 - x)); /* reflection formula */
  if (x < 1.0)
    return digamma(1.0 + x) - 1.0 / x;
  if (x == 1.0)
    return -M_GAMMA;
  if (x == 2.0)
    return 1.0 - M_GAMMA;
  if (x == 3.0)
    return 1.5 - M_GAMMA;
  if (x > 3.0)
    return 0.5 * (digamma(x / 2.0) + digamma((x + 1.0) / 2.0)) + M_LN2;
  register double Tn_1 = 1.0;   /* T_{n-1}(x), started at n=1 */
  register double Tn = x - 2.0; /* T_{n}(x) , started at n=1 */
  register double resul = Kncoe[0] + Kncoe[1] * Tn;
  x -= 2.0;

  for (int n = 2; n < sizeof(Kncoe) / sizeof(double); ++n) {
    const double Tn1 = 2.0 * x * Tn - Tn_1;
    resul += Kncoe[n] * Tn1;
    Tn_1 = Tn;
    Tn = Tn1;
  }

  return resul;
}

double gamma_initial_guess(const double x) {
  if (x > 15.0) {
    // Решение уравнения Стирлинга: x ≈ y^y / e^y * sqrt(y)
    const double y = log(x) / (log(log(x)) - 1.0);
    return fmax(y, 1.461632); // Минимум гамма-функции (~1.461632)
  }
  if (x >= 1.0)
    return 2.0;
  return (x < 0.8856) ? 0.5 : 1.5; // gamma(0.5)=1.77245, gamma(1.5)=0.886227
}

double dgamma(const double x) { return tgamma(x) * digamma(x); }

double arcgamma(double x, bool *success) {
  *success = true;
  if (x < 0.0 || isnan(x)) {
    *success = false;
    return NAN;
  }
  if (x == 0.0) { // gamma(y) → ∞ при y → 0+
    *success = false;
    return NAN;
  }

  double y = gamma_initial_guess(x);
  int iter = 0;
  double prev_y = y;

  do {
    const double gamma_val = tgamma(y);
    const double delta = gamma_val - x;

    if (fabs(delta) < GAMMA_EPSILON * fabs(x)) {
      return y;
    }

    if (y <= 0.0 || isinf(gamma_val)) {
      *success = false;
      return NAN;
    }

    const double derivative = dgamma(y);
    if (fabs(derivative) < DBL_EPSILON)
      break;

    const double step = delta / derivative;
    y = fmax(y - fmax(fmin(step, 0.5 * y), -0.5 * y), 1e-4);

    if (fabs(y - prev_y) < 1e-15 * fabs(y))
      break;
    prev_y = y;
  } while (++iter < MAX_ITERATIONS);

  *success = (fabs(tgamma(y) - x) < GAMMA_EPSILON);
  return *success ? y : NAN;
}

double arcfactorial(const double x, bool *success) {
  bool gamma_success = true;
  const double gamma_x = arcgamma(x, &gamma_success);
  if (!gamma_success) {
    *success = false;
    return NAN;
  }

  return gamma_x - 1.0f;
}
