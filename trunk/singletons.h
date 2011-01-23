#ifndef SINGLETONS_H
#define SINGLETONS_H

#include <QPlainTextEdit>

class BFLogger
{
private:
	BFLogger() : m_pte(0) {}
	BFLogger(const BFLogger &other) { Q_UNUSED(other); }
public:
	static BFLogger &instance() {
		static BFLogger logger;
		return logger;
	}
	void setPlainTextEdit(QPlainTextEdit *pte) { m_pte = pte; }
	void log(const QString &string) {
		m_pte->appendPlainText(string);
	}

private:
	QPlainTextEdit *m_pte;
};

class ACSLogger
{
private:
	ACSLogger() : m_pte(0) {}
	ACSLogger(const BFLogger &other) { Q_UNUSED(other); }
public:
	static ACSLogger &instance() {
		static ACSLogger logger;
		return logger;
	}

	void setPlainTextEdit(QPlainTextEdit *pte) { m_pte = pte; }
	void log(const QString &string) {
		m_pte->appendPlainText(string);
	}
private:
	QPlainTextEdit *m_pte;
};

class ACSParameters
{
private:
	ACSParameters() : m_beta(0.6), m_phi(0.9), m_pheromone0(10) {}
	ACSParameters(const ACSParameters &other) { Q_UNUSED(other); }
public:
	static ACSParameters &instance() {
		static ACSParameters params;
		return params;
	}

	void setBeta(double b) {
		m_beta = b;
	}

	double beta() const {
		return m_beta;
	}

	void setPhi(double p) {
		m_phi = p;
	}

	double phi() const {
		return m_phi;
	}

	void setPheromoneZero(int p0) {
		m_pheromone0 = p0;
	}

	int pheromoneZero() const {
		return m_pheromone0;
	}

private:
	double m_beta;
	double m_phi;
	int m_pheromone0;
};

#endif // SINGLETONS_H
