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

#endif // SINGLETONS_H
