import math


def polar_coordinates_ctrl(rho, gamma, delta, k1=1.0, k2=3.0, k3=2.0):
    # see https://www.diag.uniroma1.it/~labrob/pub/papers/CST02.pdf
    # see https://www.researchgate.net/publication/225543929_Control_of_Wheeled_Mobile_Robots_An_Experimental_Overview
    # see https://www.researchgate.net/profile/Giuseppe-Oriolo-3/publication/3332395_WMR_control_via_dynamic_feedback_linearization_Design_implementation_and_experimental_validation/links/5480795f0cf2ccc7f8bd061f/WMR-control-via-dynamic-feedback-linearization-Design-implementation-and-experimental-validation.pdf
    v = k1 * rho * math.cos(gamma)
    w = k2 * gamma + k1 * (math.sin(gamma) * math.cos(gamma)) / gamma * (gamma + k3 * delta)
    return (v, w)
