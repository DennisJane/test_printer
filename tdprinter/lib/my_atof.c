int my_isspace(const char c){
	switch(c){
		case '\r':
		case '\n':
		case ' ':
		case '\0':
		case '\t':
			return 1;
		default:
			break;
	}
	return 0;
}

double my_strtod(const char* s, char** endptr) {
	const char*  p     = s;
	long double  value = 0.L;
	int                   sign  = 0;
	long double           factor;
	unsigned int          expo;
	while ( my_isspace(*p) )//����ǰ��Ŀո�
		p++;
	if(*p == '-' || *p == '+')
		sign = *p++;//�ѷ��Ÿ����ַ�sign��ָ����ơ�
	//���������ַ�
	while ( (unsigned int)(*p - '0') < 10u )//ת����������
		value = value*10 + (*p++ - '0');
	//����������ı�ʾ��ʽ���磺1234.5678��
	if ( *p == '.' ) {
		factor = 1.;
		p++;
		while ( (unsigned int)(*p - '0') < 10u ) {
			factor *= 0.1;
			value  += (*p++ - '0') * factor;
		}
	}
	//�����IEEE754��׼�ĸ�ʽ���磺1.23456E+3��
	if ( (*p | 32) == 'e' ) {
		expo   = 0;
		factor = 10.L;
		switch (*++p) {
		case '-':
			factor = 0.1;
		case '+':
			p++;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;
		default :
			value = 0.L;
			p     = s;
			goto done;
		}
		while ( (unsigned int)(*p - '0') < 10u )
			expo = 10 * expo + (*p++ - '0');
		while ( 1 ) {
			if ( expo & 1 )
				value *= factor;
			if ( (expo >>= 1) == 0 )
				break;
			factor *= factor;
		}
	}
done:
	if ( endptr != 0 )
		*endptr = (char*)p;
	return (sign == '-' ? -value : value);
}
double my_atof(char *str) {
	return my_strtod(str,0);
}
